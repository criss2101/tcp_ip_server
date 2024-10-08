#include "../inc/BaseSocket.h"
#include <arpa/inet.h>

namespace Sockets
{
    void Sockets::BaseSocket::Init()
    {
        address_.sin_family = domain_;
        address_.sin_port = htons(port_);
        if(inet_pton(AF_INET, ip_address_.c_str(), &address_.sin_addr.s_addr) <= 0)
        {
            perror("inet_pton failure");
            exit(EXIT_FAILURE);
        }

        if ((socket_handler_ = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            perror("socket failure");
            exit(EXIT_FAILURE);
        }

        if ((connection_ = establish_connection()) == 0)
        {
            perror("establish_connection failure");
            exit(EXIT_FAILURE);
        }

    }
}