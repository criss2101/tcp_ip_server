#include <iostream>
#include <cstring>
#include <cstdint>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <csignal>

int socket_fd = -1;

void signalHandler(int signal)
{
    if (signal == SIGINT)
    {
        std::cout << "Closing connection. \n";
        close(socket_fd);
        exit(0);
    }
}

int main()
{
    std::signal(SIGINT, signalHandler);

    while(true)
    {
        sockaddr_in server;

        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd == -1)
        {
            perror("Failed to create socket");
            return 1;
        }

        server.sin_family = AF_INET;
        server.sin_port = htons(2345);
        server.sin_addr.s_addr = inet_addr("127.0.0.1");


        if (connect(socket_fd, reinterpret_cast<const sockaddr*>(&server), sizeof(server)) < 0)
        {
            perror("Failed to establish connection");
            close(socket_fd);
            return 1;
        }


        uint32_t data;
        std::string text;

        std::cout << "Enter command number:";
        std::cin >> data;
        send(socket_fd, reinterpret_cast<const char*>(&data), sizeof(data), 0);

        std::cout << "Enter payload size: ";
        std::cin >> data;
        send(socket_fd, reinterpret_cast<const char*>(&data), sizeof(data), 0);

        std::cout << "Enter string data: ";
        std::cin.ignore();
        std::getline(std::cin, text);
        std::cout << "text.size(): " << text.size() << std::endl;
        send(socket_fd, text.c_str(), text.size(), 0);

        std::cout << "Click any button to send uint32_t (0).\n";
        std::cin.ignore();
        data = 0;
        send(socket_fd, reinterpret_cast<const char*>(&data), sizeof(data), 0);

        std::cout << "Closing packet uint32_t (0) has been sent." << std::endl;
    }

    close(socket_fd);
    return 0;
}
