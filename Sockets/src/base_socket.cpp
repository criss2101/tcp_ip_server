#include <arpa/inet.h>
#include <unistd.h>

#include "../inc/base_socket.h"

namespace Sockets
{
    BaseSocket::BaseSocket(const int domain, const int type, const int protocol, std::string ip_adress, const uint16_t port, const int waking_up_timeout)
    : domain_{domain}, type_{type}, protocol_{protocol}, ip_address_{std::move(ip_adress)}, port_{port}, waking_up_timeout_{waking_up_timeout}
        {}

    BaseSocket::~BaseSocket()
    {
        Close();
    }

    void BaseSocket::SetSocketTimeout(const int seconds)
    {
        timeval timeout;
        timeout.tv_sec = seconds;
        timeout.tv_usec = 0;

         if (setsockopt(socket_fd_, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeout), sizeof(timeout)) < 0)
         {
            perror("setsockopt(SO_RCVTIMEO) failed");
         }
    }

    void BaseSocket::ExitWithError(const char *message)
    {
        perror(message);
        exit(EXIT_FAILURE);
    }

    void BaseSocket::Init()
    {
        address_.sin_family = domain_;
        address_.sin_port = htons(port_);
        if(inet_pton(AF_INET, ip_address_.c_str(), &address_.sin_addr.s_addr) <= 0)
        {
            ExitWithError("inet_pton failure");
        }

        if ((socket_fd_ = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            ExitWithError("socket failure");
        }

        SetSocketTimeout(10);

        if (EstablishConnection() < 0)
        {
            ExitWithError("EstablishConnection failure");
        }

        is_open = true;
    }

    void BaseSocket::Close()
    {
        close(socket_fd_);
        is_open = false;
    }
} // namespace Sockets

