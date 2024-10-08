#include "../inc/tcp_ip_server.h"
#include "tcp_ip_server.h"
#include "../../Sockets/inc/listening_socket.h"

namespace Server
{
    TcpIpServer::TcpIpServer(const Config::TcpIpServerConfig config)
    {
        listening_socket_ = std::make_unique<Sockets::ListeningSocket>(config.socket_domain, config.socket_type, config.socket_protocol, config.ip_adress, config.socket_port);
    }

    void TcpIpServer::LaunchServer()
    {
        listening_socket_->Init();
        listening_socket_->StartListening();
        is_running_ = true;
    }

    void TcpIpServer::ShutdownServer()
    {
        listening_socket_->Close();
        is_running_ = false;
    }

} // namespace Server
