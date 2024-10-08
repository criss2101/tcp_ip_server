#include "Server/inc/tcp_ip_server.h"
#include <arpa/inet.h>
#include <memory>

int main()
{
    Server::Config::TcpIpServerConfig config;
    config.socket_domain = AF_INET;
    config.socket_type = SOCK_STREAM;
    config.socket_protocol = 0;
    config.ip_adress = "127.0.0.1";
    config.socket_port = 8080;
    config.max_connections = 10;

    std::unique_ptr<Server::Interface::ITcpIpServer> server = std::make_unique<Server::TcpIpServer>(config);

    server->LaunchServer();


    return 0;
}
