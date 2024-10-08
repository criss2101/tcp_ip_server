#ifndef SOCKETS_INC_LISTENING_SOCKET_H
#define SOCKETS_INC_LISTENING_SOCKET_H

#include "base_socket.h"

namespace Sockets
{
    class ListeningSocket : public Sockets::BaseSocket
    {
        public:
        void print();

        int establish_connection() override
        {
            // TODO

            return 0;
        }
    };
}

#endif // SOCKETS_INC_LISTENING_SOCKET_H
