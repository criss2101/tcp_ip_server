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

    int32_t command_id;
    std::cout << "Enter command number: ";
    std::cin >> command_id;
    send(socket_fd, reinterpret_cast<const char*>(&command_id), sizeof(command_id), 0);

    int32_t payload_size;
    std::cout << "Enter payload size: ";
    std::cin >> payload_size;
    send(socket_fd, reinterpret_cast<const char*>(&payload_size), sizeof(payload_size), 0);

    if(command_id == 1)
    {
        std::string str;
        std::cout << "Enter payload data [string]: ";
        std::cin.ignore();
        std::getline(std::cin, str);
        size_t length = str.size();
        int8_t* byte_array = new int8_t[length];
        std::memcpy(byte_array, str.data(), length);
        byte_array[length] = '\0';
        send(socket_fd, byte_array, length, 0);
        delete[] byte_array;
    }
    else if(command_id == 2)
    {
        int32_t number;
        std::cout << "Enter payload data [number]: ";
        std::cin >> number;
        send(socket_fd, reinterpret_cast<const char*>(&number), sizeof(number), 0);
    }

    int32_t ending_mark;
    std::cout << "Provide 0 to send ending mark: \n";
    std::cin >> ending_mark;
    send(socket_fd, reinterpret_cast<const char*>(&ending_mark), sizeof(ending_mark), 0);

    std::cout << "Closing packet int32_t (0) has been sent." << std::endl;


    close(socket_fd);
    return 0;
}
