#include <sys/epoll.h>
#include <unistd.h>
#include "../inc/tcp_ip_server.h"
#include "../../Sockets/inc/listening_socket.h"

namespace Server
{
    TcpIpServer::TcpIpServer(const Config::TcpIpServerConfig config)
    {
        listening_socket_ = std::make_unique<Sockets::ListeningSocket>(config.socket_domain, config.socket_type, config.socket_protocol, config.ip_adress, config.socket_port, config.max_connections);
    }

    TcpIpServer::~TcpIpServer()
    {
        ShutdownServer();
    }

    void TcpIpServer::LaunchServer()
    {
        listening_socket_->Init();
        listening_socket_->StartListening();
        is_running_ = true;

        Work();
    }

    void TcpIpServer::ShutdownServer()
    {
        listening_socket_->Close();
        is_running_ = false;
    }

    void TcpIpServer::Work()
    {
        epoll_event event, events[max_events_];
        const auto& socket_fd = listening_socket_->GetSocketFd();

        // Create epoll instance
        auto epollFd = epoll_create1(0);
        if (epollFd == -1) {
            perror("epol failure");
            exit(EXIT_FAILURE);
        }

        // Add server socket to epoll
        event.events = EPOLLIN;
        event.data.fd = socket_fd;
        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, socket_fd, &event) == -1) {
            perror("epol failure");
            exit(EXIT_FAILURE);
        }

        while (true) {
            int numEvents = epoll_wait(epollFd, events, max_events_, -1);
            if (numEvents == -1) {
                perror("Failed to wait for events.");
                break;
            }

            for (int i = 0; i < numEvents; ++i) {
                if (events[i].data.fd == socket_fd)
                {
                    // Accept new client connection
                    sockaddr_in clientAddress;
                    socklen_t clientAddressLength = sizeof(clientAddress);
                    int clientFd = accept(socket_fd, (sockaddr*)&clientAddress, &clientAddressLength);
                    if (clientFd == -1) {
                        perror("Failed to wait for events.");
                        continue;
                    }

                    // Add client socket to epoll
                    event.events = EPOLLIN;
                    event.data.fd = clientFd;
                    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &event) == -1) {
                        perror("Failed to add client socket to epoll instance.");
                        close(clientFd);
                        continue;
                    }
                }
                else
                {
                    char buffer[1024];
                    ssize_t count;
                    while ((count = read(events[i].data.fd, buffer, sizeof(buffer))) > 0)
                    {
                        write(STDOUT_FILENO, buffer, count);
                    }
                    if (count == -1 && errno != EAGAIN) {
                        perror("Read failure");
                        close(events[i].data.fd);
                    } else if (count == 0) {
                        close(events[i].data.fd);
                    }
                }
            }
        }
    }

} // namespace Server
