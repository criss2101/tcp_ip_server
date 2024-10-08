#include "listening_socket.h"

namespace Sockets
{
    int ListeningSocket::EstablishConnection()
    {
        return bind(socket_fd_, reinterpret_cast<const sockaddr*>(&address_), sizeof(address_));
    }
}
