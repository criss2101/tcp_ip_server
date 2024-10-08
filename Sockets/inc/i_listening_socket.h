#ifndef SOCKETS_INC_I_LISTENING_SOCKET_H
#define SOCKETS_INC_I_LISTENING_SOCKET_H

namespace Sockets
{
    namespace Interface
    {
        class IListeningSocket
        {
            public:
            virtual ~IListeningSocket() = default;

            virtual void Init() = 0;
            virtual void StartListening() = 0;
            virtual int Accept() = 0;
            virtual void Close() = 0;
            virtual bool IsOpen() const = 0;   
        };
    } // namespace Interface
} // namespace Sockets

#endif // SOCKETS_INC_I_LISTENING_SOCKET_H
