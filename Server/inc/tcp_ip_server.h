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
            virtual void LaunchServer() = 0;
            virtual void ShutdownServer() = 0;
            virtual bool IsRunning() const = 0;
        };
    } // namespace Interface

    class TcpIpServer : public Interface::ITcpIpServer
    {
        public:
        explicit TcpIpServer(Config::TcpIpServerConfig config);
        ~TcpIpServer();

        void LaunchServer() override;
        void ShutdownServer() override;
        bool IsRunning() const override { return is_running_; };

        private:
        void HandleClientData(int client_fd);
        void RemoveSocketFdFromMap(const int socket_fd);
        void Work();
        void OnClientDataReceived(const int client_fd);

        std::atomic<bool> is_running_{false};
        std::unique_ptr<Sockets::Interface::IListeningSocket> listening_socket_;
        const int max_events_{10};
        const int timeout_sec_{60};

        std::unordered_map<int, bool> client_fd_to_processing_state_map_;
        std::mutex map_access_mutex_;

        std::unique_ptr<Server::Processing::Interface::ICommandProcessor> command_processor_;
     };

} // namespace Server

#endif // SERVER_INC_TCP_IP_SERVER_H
