#ifndef SOCKETS_INC_CLIENT_SOCKET_H
#define SOCKETS_INC_CLIENT_SOCKET_H

#include "base_socket.h"

namespace Sockets
{
    // Just a example class, might be used as a core module for client program connection
    class ClientSocket : public BaseSocket
    {
        private:
        int EstablishConnection() override
        {
            return connect(socket_fd_, reinterpret_cast<const sockaddr*>(&address_), sizeof(address_));
        }
    };
} // namespace Sockets

#endif // SOCKETS_INC_CLIENT_SOCKET_H
