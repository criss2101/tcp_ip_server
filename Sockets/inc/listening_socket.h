#ifndef SOCKETS_INC_LISTENING_SOCKET_H
#define SOCKETS_INC_LISTENING_SOCKET_H

#include "base_socket.h"

namespace Sockets
{
    class ListeningSocket : public BaseSocket
    {
        private:
        int EstablishConnection() override;
    };
}

#endif // SOCKETS_INC_LISTENING_SOCKET_H
