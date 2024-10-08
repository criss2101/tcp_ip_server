#ifndef SOCKETS_INC_CLIENT_SOCKET_H
#define SOCKETS_INC_CLIENT_SOCKET_H

#include "base_socket.h"

namespace Sockets
{
    class ClientSocket : public BaseSocket
    {
        private:
        int EstablishConnection() override
        {
            return connect(socket_fd_, reinterpret_cast<const sockaddr*>(&address_), sizeof(address_));
        }
    };
}

#endif // SOCKETS_INC_CLIENT_SOCKET_H
