#ifndef SERVER_INC_TCP_IP_SERVER_H
#define SERVER_INC_TCP_IP_SERVER_H

#include <string>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <atomic>
#include "../../Sockets/inc/i_listening_socket.h"
#include "command_processor.h"

namespace Server
{
    namespace Config
    {
        struct TcpIpServerConfig
        {
            int socket_domain{};
            int socket_type{};
            int socket_protocol{};
            int socket_waking_up_timeout{};
            std::string ip_adress{};
            uint16_t socket_port{};
            int max_connections{};
        };
    } // namespace Config

    // Can be extracted to separate files
    namespace Interface
    {
        class ITcpIpServer
        {
            public:
            virtual ~ITcpIpServer() = default;

            /**
             * @brief Launches the server.
             */
            virtual void LaunchServer() = 0;

            /**
             * @brief Shuts down the server.
             */
            virtual void ShutdownServer() = 0;

            /**
             * @brief Checks if the server is currently running.
             *
             * @return true if the server is running, false otherwise.
             */
            virtual bool IsRunning() const = 0;
        };
    } // namespace Interface

    class TcpIpServer : public Interface::ITcpIpServer
    {
        public:

        explicit TcpIpServer(Config::TcpIpServerConfig config, std::atomic<bool>& is_sigint_occured);

        TcpIpServer(const TcpIpServer&) = delete;
        TcpIpServer& operator=(const TcpIpServer&) = delete;
        TcpIpServer(TcpIpServer&&) = delete;
        TcpIpServer& operator=(TcpIpServer&&) = delete;

        ~TcpIpServer();

        void LaunchServer() override;
        void ShutdownServer() override;
        bool IsRunning() const override { return is_running_ && !is_sigint_occured; }

        private:
        /**
         * @brief Handles the data from a specific client_fd.
         *
         * @param client_fd The file descriptor of the client socket.
         */
        void HandleClientData(int client_fd);

        /**
         * @brief Removes a client's socket file descriptor from the internal tracking map.
         * This is typically done when a client disconnects or when an error occurs.
         *
         * @param socket_fd The file descriptor of the client's socket.
         */
        void RemoveSocketFdFromMap(const int socket_fd);

        /**
         * @brief Main worker function responsible for managing the server's operations.
         */
        void Work();

        /**
         * @brief Called when epoll event occurs.
         * It starting the reading thread.
         *
         * @param client_fd The file descriptor of the client socket.
         */
        void OnClientDataReceived(const int client_fd);

        /**
         * @brief Handles errors that occur during reading data from a client.
         * Cleans opened connections.
         * This could be triggered by socket errors or unexpected conditions.
         *
         * @param client_fd The file descriptor of the client socket.
         */
        void OnReadError(const int client_fd);

        std::atomic<bool> is_running_{false};
        std::atomic<bool>& is_sigint_occured;
        std::unique_ptr<Sockets::Interface::IListeningSocket> listening_socket_;
        const int max_events_{10};
        const int timeout_sec_{20};

        std::unordered_map<int, bool> client_fd_to_processing_state_map_;
        std::mutex map_access_mutex_;

        std::unique_ptr<Server::Processing::Interface::ICommandProcessor> command_processor_;
     };

} // namespace Server

#endif // SERVER_INC_TCP_IP_SERVER_H

