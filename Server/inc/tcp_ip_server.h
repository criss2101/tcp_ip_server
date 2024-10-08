#ifndef SERVER_INC_TCP_IP_SERVER_H
#define SERVER_INC_TCP_IP_SERVER_H

#include <string>
#include <memory>
#include "../../Sockets/inc/i_listening_socket.h"

namespace Server
{
    namespace Config
    {
        struct TcpIpServerConfig
        {
            const int socket_domain;
            const int socket_type;
            const int socket_protocol;
            std::string ip_adress;
            const uint16_t socket_port;
            const int max_connections_;
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

        void LaunchServer() override;
        void ShutdownServer() override;
        bool IsRunning() const override { return is_running_; };

        private:
        bool is_running_{false};
        std::unique_ptr<Sockets::Interface::IListeningSocket> listening_socket_;
     };

} // namespace Server

#endif // SERVER_INC_TCP_IP_SERVER_H