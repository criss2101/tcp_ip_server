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
        explicit BaseSocket(const int domain, const int type, const int protocol, std::string ip_adress, const uint16_t port) : 
        domain_{domain}, type_{type}, protocol_{protocol}, ip_address_{std::move(ip_adress)}, port_{port} {}

        protected:
        void Init();
        virtual int EstablishConnection() = 0;

        sockaddr_in address_;
        int socket_fd_;

        private:
        // TODO: Analyse what can be removed from members
        const int domain_;
        const int type_;
        const int protocol_;
        const std::string ip_address_;
        const uint16_t port_;
    };
}

#endif // SOCKETS_INC_BASE_SOCKET_H
