#ifndef SOCKETS_INC_LISTENING_SOCKET_H
#define SOCKETS_INC_LISTENING_SOCKET_H

#include "BaseSocket.h"

namespace Sockets
{
    class ListeningSocket : public Sockets::BaseSocket
    {
        public:
        void print();

        int establish_connection() override
        {
            // TODO
        }
    };
}

#endif // SOCKETS_INC_LISTENING_SOCKET_H
