#ifndef SOCKETS_INC_LISTENING_SOCKET_H
#define SOCKETS_INC_LISTENING_SOCKET_H

#include "base_socket.h"
#include "i_listening_socket.h"

namespace Sockets
{
    class ListeningSocket final : public BaseSocket, Interface::IListeningSocket
    {
        public:
        explicit ListeningSocket(const int domain, const int type, const int protocol, std::string ip_adress, const uint16_t port);

        private:
        int EstablishConnection() override;

        void Init() override;
        void StartListening() override;
        int Accept() override;
        void Close() override;
        bool IsOpen() const override {return is_open; }
        
        const int max_connections_{10};
    };
} // namespace Sockets

#endif // SOCKETS_INC_LISTENING_SOCKET_H
