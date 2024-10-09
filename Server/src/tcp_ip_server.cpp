#include <sys/epoll.h>
#include <unistd.h>
#include <chrono>
#include <iostream>
#include <vector>
#include <cstddef>
#include "../inc/tcp_ip_server.h"
#include "../../Sockets/inc/listening_socket.h"


namespace Server
{
    namespace
    {
        bool TimeoutReached(const std::chrono::steady_clock::time_point& current_time, const std::chrono::steady_clock::time_point& start_time, const int timeout_sec)
        {
            const std::chrono::duration<double> elapsed_time = current_time - start_time;
            if (elapsed_time.count() > timeout_sec) {
                std::cerr << "Error: Timeout while waiting for header_buf." << std::endl;
                return true;
            }
            return false;
        }
    }

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

        std::cout<<"Server initialised. \n";
        Work();
    }

    void TcpIpServer::ShutdownServer()
    {
        listening_socket_->Close();
        is_running_ = false;
    }

    void TcpIpServer::Work()
    {
        // Creation of epoll might be wrapped into some external module
        epoll_event event, events[max_events_];
        const auto& socket_fd = listening_socket_->GetSocketFd();

        // Create epoll instance
        auto epollFd = epoll_create1(0);
        if (epollFd == -1)
        {
            perror("epoll_create1 failure");
            exit(EXIT_FAILURE);
        }

        // Add server socket to epoll
        event.events = EPOLLIN;
        event.data.fd = socket_fd;
        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, socket_fd, &event) == -1)
        {
            perror("epoll_ctl failure");
            exit(EXIT_FAILURE);
        }

        while (true)
        {
            int numEvents = epoll_wait(epollFd, events, max_events_, -1);
            if (numEvents == -1) {
                perror("Failed to wait for events.");
                break;
            }

            for (int i = 0; i < numEvents; ++i)
            {
                if (events[i].data.fd == socket_fd)
                {
                    sockaddr_in clientAddress;
                    socklen_t clientAddressLength = sizeof(clientAddress);
                    int clientFd = accept(socket_fd, (sockaddr*)&clientAddress, &clientAddressLength);
                    if (clientFd == -1)
                    {
                        perror("Failed to wait for events.");
                        continue;
                    }
                    std::cout<<"\n\nNew connection accepted. \n";

                    // Add client socket to epoll
                    event.events = EPOLLIN;
                    event.data.fd = clientFd;
                    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &event) == -1)
                    {
                        perror("Failed to add client socket to epoll instance.");
                        close(clientFd);
                        continue;
                    }
                }
                else
                {
                    HandleClientData(events[i].data.fd);
                }
            }
        }
    }

    void Server::TcpIpServer::HandleClientData(int client_fd)
    {
        auto start_time = std::chrono::steady_clock::now();
        std::cout<<"New data received from client_fd: "<< client_fd << "\n";

        // Header data reading - should be extracted to some Reader class, to make it possible to unit test
        constexpr int header_size{sizeof(uint32_t) * 2};
        std::array<std::byte, header_size> header_buf{};
        size_t total_bytes_received = 0;

        while (total_bytes_received < header_size)
        {
            const ssize_t bytesRead = read(client_fd, header_buf.data() + total_bytes_received, header_size - total_bytes_received);
            if (bytesRead <= 0)
            {
                std::cerr << "ERROR\n";
                perror("Header data reading - Error reading from client or connection closed. Disconnecting client.");
                close(client_fd);
                return;
            }
            total_bytes_received += bytesRead;

            const auto currentTime = std::chrono::steady_clock::now();
            if(TimeoutReached(currentTime, start_time, timeout_sec_))
            {
                std::cerr << "ERROR\n";
                std::cerr << "Header data reading - Timeout. Disconnecting client.\n";
                close(client_fd);
                return;
            }

            std::cout << "Header total_bytes_received: " << total_bytes_received << " < " << header_size << "\n";
        }

        const u_int32_t command_id = *(reinterpret_cast<u_int32_t*>(header_buf.data()));
        const u_int32_t payload_size = *(reinterpret_cast<u_int32_t*>(header_buf.data() + sizeof(u_int32_t)));
        std::cout << "Received Command ID: " << command_id << ", Payload Size: " << payload_size << std::endl;
        // end Header data reading

        // Payload data reading - should be extracted to some reader class, to make it possible to unit test
        std::vector<std::byte> payload(payload_size);
        total_bytes_received = 0;
        if (payload_size > 0)
        {
            while (total_bytes_received < payload_size)
            {
                ssize_t bytesRead = read(client_fd, payload.data() + total_bytes_received, payload_size - total_bytes_received);
                if (bytesRead <= 0)
                {
                    perror("Payload data reading - Error reading payload from client or connection closed. Disconnecting client.");
                    close(client_fd);
                    return;
                }
                total_bytes_received += bytesRead;

                const auto currentTime = std::chrono::steady_clock::now();
                if(TimeoutReached(currentTime, start_time, timeout_sec_))
                {
                    std::cerr << "ERROR\n";
                    std::cerr << "Payload data reading - Timeout. Disconnecting client.\n";
                    close(client_fd);
                    return;
                }

                std::cout << "Payload total_bytes_received: " << total_bytes_received << " < " << payload_size << "\n";
            }
        }
        else
        {
            std::cerr << "ERROR\n";
            std::cerr << "Payload = 0, no need to process command. Disconnecting client. \n";
            close(client_fd);
            return;
        }

        // Ending mark reading - should be extracted to some Reader class, to make it possible to unit test
        constexpr int ending_mark_size{sizeof(uint32_t)};
        std::array<std::byte, ending_mark_size> ending_mark_buf{};
        total_bytes_received = 0;

        while (total_bytes_received < ending_mark_size)
        {
            ssize_t bytesRead = read(client_fd, ending_mark_buf.data(), ending_mark_size - total_bytes_received);
            if (bytesRead <= 0)
            {
                std::cerr << "ERROR\n";
                std::cerr << "Ending mark reading - 0 bytes received. Check whether payload size and actual payload data are correct.  Disconnecting client.\n";
                close(client_fd);
                return;
            }
            total_bytes_received += bytesRead;

            const auto currentTime = std::chrono::steady_clock::now();
            if(TimeoutReached(currentTime, start_time, timeout_sec_))
            {
                std::cerr << "ERROR\n";
                std::cerr << "Ending mark reading - Timeout. Disconnecting client.\n";
                close(client_fd);
                return;
            }
        }
        // end Ending mark reading


        close(client_fd);
    }

} // namespace Server
