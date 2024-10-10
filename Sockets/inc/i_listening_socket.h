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

            /**
             * @brief Initializes the socket.
             */
            virtual void Init() = 0;

            /**
             * @brief Starts listening for incoming connections.
             */
            virtual void StartListening() = 0;

            /**
             * @brief Accepts an incoming client connection and returns the client's socket file descriptor.
             * @return The file descriptor for the accepted client socket, or -1 on error.
             */
            virtual int Accept() = 0;

            /**
             * @brief Closes the listening socket.
             */
            virtual void Close() = 0;

            /**
             * @brief Checks whether the socket is open.
             * @return True if the socket is open, otherwise false.
             */
            virtual bool IsOpen() const = 0;

            /**
             * @brief Returns the file descriptor of the listening socket.
             * @return The socket file descriptor.
             */
            virtual int GetSocketFd() const = 0;
        };
    } // namespace Interface
} // namespace Sockets

#endif // SOCKETS_INC_I_LISTENING_SOCKET_H
