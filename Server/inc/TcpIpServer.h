#ifndef SERVER_INC_TCP_IP_SERVER_H
#define SERVER_INC_TCP_IP_SERVER_H

#include "../../Sockets/inc/BaseSocket.h"

namespace Server
{
    class TcpIpServer
    {
        public:
        void print();

        private:
        Sockets::BaseSocket bs;
    };
} // Server

#endif // SERVER_INC_TCP_IP_SERVER_H