#ifndef SOCKETS_INC_LISTENING_SOCKET_H
#define SOCKETS_INC_LISTENING_SOCKET_H

#include "BaseSocket.h"

namespace Sockets
{
    class ListeningSocket : public Sockets::BaseSocket
    {
        public:
        void print();
    };
}

#endif // SOCKETS_INC_LISTENING_SOCKET_H
