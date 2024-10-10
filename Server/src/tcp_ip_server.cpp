#include <sys/epoll.h>
#include <unistd.h>
#include <chrono>
#include <iostream>
#include <vector>
#include <cstddef>
#include <thread>
#include <variant>
#include <csignal>
#include <iomanip>
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

        void HandleResult(const Processing::Interface::CommandResult& result)
        {
            std::visit([](auto&& arg)
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, double>)
                {
                    std::cout << "OK: " << std::setprecision(10) << arg << std::endl;
                }
                else if constexpr (std::is_same_v<T, std::string>)
                {
                    std::cout << "OK: " << arg << std::endl;
                }
                else
                {
                    std::cout << "ERROR\n";
                }
            }, result);
        }
    }

    TcpIpServer::TcpIpServer(const Config::TcpIpServerConfig config)
    : listening_socket_{std::make_unique<Sockets::ListeningSocket>(config.socket_domain, config.socket_type, config.socket_protocol, config.ip_adress, config.socket_port, config.max_connections, config.socket_waking_up_timeout)},
      command_processor_{std::make_unique<Processing::CommandProcessor>()}
    { }

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
        std::cout<<"Server turning off. \n";
        listening_socket_->Close();
        is_running_ = false;
    }

    void TcpIpServer::Work()
    {
        // Creation of epoll might be wrapped into some external module
        epoll_event event, events[max_events_];
        const auto& server_socket_fd = listening_socket_->GetSocketFd();

        // Create epoll instance
        auto epoll_fd = epoll_create1(0);
        if (epoll_fd == -1)
        {
            perror("epoll_create1 failure");
            exit(EXIT_FAILURE);
        }

        // Add server socket to epoll
        event.events = EPOLLIN;
        event.data.fd = server_socket_fd;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket_fd, &event) == -1)
        {
            perror("epoll_ctl failure");
            exit(EXIT_FAILURE);
        }

        std::cout<<"Server started working... \n";

        while (true)
        {
            int numEvents = epoll_wait(epoll_fd, events, max_events_, -1);
            if (numEvents == -1) {
                perror("Failed to wait for events.");
                break;
            }

            for (int i = 0; i < numEvents; ++i)
            {
                if (events[i].data.fd == server_socket_fd)
                {
                    sockaddr_in client_address;
                    socklen_t client_address_length = sizeof(client_address);
                    int new_client_fd = accept(server_socket_fd, (sockaddr*)&client_address, &client_address_length);
                    if (new_client_fd == -1)
                    {
                        perror("Failed to wait for events.");
                        continue;
                    }
                    std::cout<<"\n\nNew connection accepted. Client_fd: " << new_client_fd << std::endl;

                    // Add client socket to epoll
                    event.events = EPOLLIN;
                    event.data.fd = new_client_fd;
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_client_fd, &event) == -1)
                    {
                        perror("Failed to add client socket to epoll instance.");
                        close(new_client_fd);
                        continue;
                    }

                    {
                        std::lock_guard<std::mutex> lock(map_access_mutex_);
                        client_fd_to_processing_state_map_.insert({new_client_fd, false});
                    }
                }
                else
                {
                    OnClientDataReceived(events[i].data.fd);
                }
            }
        }
    }

    void TcpIpServer::OnClientDataReceived(const int client_fd)
    {
        {
            std::lock_guard<std::mutex> lock(map_access_mutex_);
            auto it = client_fd_to_processing_state_map_.find(client_fd);
            if ((it != client_fd_to_processing_state_map_.end() && it->second) || it == client_fd_to_processing_state_map_.end())
            {
                return;
            }
            client_fd_to_processing_state_map_[client_fd] = true;
        }

        std::thread clientDataProcessingThread(&TcpIpServer::HandleClientData, this, client_fd);
        clientDataProcessingThread.detach();
    }

    void TcpIpServer::OnReadError(const int client_fd)
    {
        close(client_fd);
        RemoveSocketFdFromMap(client_fd);
    }

    void TcpIpServer::HandleClientData(int client_fd)
    {
        auto start_time = std::chrono::steady_clock::now();
        auto current_time = std::chrono::steady_clock::now();
        std::cout<<"New data received from client_fd: "<< client_fd << "\n";

        // Header data reading - should be extracted to some Reader class, to make it possible to unit test
        constexpr int header_size{sizeof(int32_t) * 2};
        std::array<int8_t, header_size> header_buf{};
        int total_bytes_received = 0;

        while (total_bytes_received < header_size && is_running_)
        {
            const ssize_t bytes_read = read(client_fd, header_buf.data() + total_bytes_received, header_size - total_bytes_received);
            if (bytes_read <= 0 && errno != EAGAIN)
            {
                std::cerr << "ERROR\n";
                perror("Header data reading - Error reading from client or connection closed. Disconnecting client.");
                OnReadError(client_fd);
                return;
            }
            if(bytes_read > 0)
            {
                total_bytes_received += bytes_read;
            }

            current_time = std::chrono::steady_clock::now();
            if(TimeoutReached(current_time, start_time, timeout_sec_))
            {
                std::cerr << "ERROR\n";
                std::cerr << "Header data reading - Timeout. Disconnecting client.\n";
                OnReadError(client_fd);
                return;
            }

            std::cout << "Client_fd: " << client_fd << " - header total_bytes_received: " << total_bytes_received << " < " << header_size << "\n";
        }

        constexpr size_t header_data_offset{sizeof(int32_t)};
        const int32_t command_id = *(reinterpret_cast<int32_t*>(header_buf.data()));
        const int32_t payload_size = *(reinterpret_cast<int32_t*>(header_buf.data() + header_data_offset));
        std::cout << "Client_fd: " << client_fd << " - received Command ID: " << command_id << ", Payload Size: " << payload_size << std::endl;

        if(!command_processor_->CommandIDSupported(static_cast<Processing::Interface::CommandID>(command_id)))
        {
            std::cerr << "ERROR\n";
            std::cerr << "Header data reading - CommandID not supported. Disconnecting client.\n";
            OnReadError(client_fd);
            return;
        }
        // end Header data reading

        // Payload data reading - should be extracted to some reader class, to make it possible to unit test
        std::vector<int8_t> payload(payload_size);
        total_bytes_received = 0;
        if (payload_size > 0)
        {
            while (total_bytes_received < payload_size && is_running_)
            {
                ssize_t bytes_read = read(client_fd, payload.data() + total_bytes_received, payload_size - total_bytes_received);
                if (bytes_read <= 0 && errno != EAGAIN)
                {
                    perror("Payload data reading - Error reading payload from client or connection closed. Disconnecting client.");
                    OnReadError(client_fd);
                    return;
                }

                if(bytes_read > 0)
                {
                    total_bytes_received += bytes_read;
                }

                current_time = std::chrono::steady_clock::now();
                if(TimeoutReached(current_time, start_time, timeout_sec_))
                {
                    std::cerr << "ERROR\n";
                    std::cerr << "Payload data reading - Timeout. Disconnecting client.\n";
                    OnReadError(client_fd);
                    return;
                }

                std::cout << "Client_fd: " << client_fd << " - payload total_bytes_received: " << total_bytes_received << " < " << payload_size << "\n";
            }
        }
        else
        {
            std::cerr << "ERROR\n";
            std::cerr << "Payload = 0, no need to process command. Disconnecting client. \n";
            OnReadError(client_fd);
            return;
        }

        // Ending mark reading - should be extracted to some Reader class, to make it possible to unit test
        constexpr int ending_mark_size{sizeof(int32_t)};
        std::array<int8_t, ending_mark_size> ending_mark_buf{};
        total_bytes_received = 0;

        while (total_bytes_received < ending_mark_size && is_running_)
        {
            ssize_t bytes_read = read(client_fd, ending_mark_buf.data(), ending_mark_size - total_bytes_received);
            if (bytes_read <= 0 && errno != EAGAIN)
            {
                std::cerr << "ERROR\n";
                std::cerr << "Ending mark reading - 0 bytes received. Check whether payload size and actual payload data are correct.  Disconnecting client.\n";
                OnReadError(client_fd);
                return;
            }

            if(bytes_read > 0)
            {
                total_bytes_received += bytes_read;
            }

            current_time = std::chrono::steady_clock::now();
            if(TimeoutReached(current_time, start_time, timeout_sec_))
            {
                std::cerr << "ERROR\n";
                std::cerr << "Ending mark reading - Timeout. Disconnecting client.\n";
                OnReadError(client_fd);
                return;
            }

            std::cout << "Client_fd: " << client_fd << " - ending mark total_bytes_received: " << total_bytes_received << " < " << ending_mark_size << "\n";
        }
        // end Ending mark reading

        if(*reinterpret_cast<int32_t*>(ending_mark_buf.data()) != 0)
        {
            std::cerr << "ERROR\n";
            std::cerr << "Ending mark incorrect. Disconnecting client.\n";
            OnReadError(client_fd);
            return;
        }

        std::cout<<"Data received, data processing start...\n";
        try
        {
            // Data Processing
            const auto result = command_processor_->ProcessCommand(static_cast<Processing::Interface::CommandID>(command_id), payload);
            HandleResult(result);
        }
        catch(const std::out_of_range& e)
        {
            std::cerr << e.what() << '\n';
            OnReadError(client_fd);
            return;
        }

        close(client_fd);
        RemoveSocketFdFromMap(client_fd);
    }

    void TcpIpServer::RemoveSocketFdFromMap(const int socket_fd)
    {
        {
            std::lock_guard<std::mutex> lock(map_access_mutex_);
            auto it = client_fd_to_processing_state_map_.find(socket_fd);
            if (it != client_fd_to_processing_state_map_.end())
            {
                client_fd_to_processing_state_map_.erase(it);
            }
        }
    }

} // namespace Server
