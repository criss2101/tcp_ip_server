#include "../inc/listening_socket.h"
#include <sys/socket.h>

namespace Sockets
{
    ListeningSocket::ListeningSocket(const int domain, const int type, const int protocol, std::string ip_adress, const uint16_t port, const int max_connections) :
        BaseSocket(domain, type, protocol, std::move(ip_adress), port), max_connections_{max_connections}
    {}

    int ListeningSocket::EstablishConnection()
    {
        return bind(socket_fd_, reinterpret_cast<const sockaddr*>(&address_), sizeof(address_));
    }

    void ListeningSocket::Init()
    {
        BaseSocket::Init();
    }

    void ListeningSocket::StartListening()
    {
        if (listen(socket_fd_, max_connections_) < 0)
        {
            BaseSocket::ExitWithError("StartingListening failue");
        }
    }

    int ListeningSocket::Accept()
    {
        return accept(socket_fd_, reinterpret_cast<sockaddr*>(&address_), reinterpret_cast<socklen_t*>(sizeof(address_)));
    }

    void ListeningSocket::Close()
    {
        BaseSocket::Close();
    }
} // namespace Sockets
