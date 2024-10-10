#ifndef SOCKETS_INC_BASE_SOCKET_H
#define SOCKETS_INC_BASE_SOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <string>

namespace Sockets
{
    class BaseSocket
    {
        public:
        explicit BaseSocket(const int domain, const int type, const int protocol, std::string ip_adress, const uint16_t port, const int waking_up_timeout);

        BaseSocket(const BaseSocket&) = delete;
        BaseSocket& operator=(const BaseSocket&) = delete;
        BaseSocket(BaseSocket&&) = delete;
        BaseSocket& operator=(BaseSocket&&) = delete;

        virtual ~BaseSocket();

        protected:
        virtual int EstablishConnection() = 0;
        void ExitWithError(const char* message);

        void Init();
        void Close();

        sockaddr_in address_;
        int socket_fd_{-1};
        bool is_open{false};

        private:
        void SetSocketTimeout(const int seconds);

        const int domain_;
        const int type_;
        const int protocol_;
        const std::string ip_address_;
        const uint16_t port_;
        const int waking_up_timeout_;
    };
} // namespace Sockets

#endif // SOCKETS_INC_BASE_SOCKET_H
