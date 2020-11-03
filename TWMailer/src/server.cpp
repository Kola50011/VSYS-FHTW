#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>

#include "spdlog/spdlog.h"
#include "CommandProcessor.h"

#define BUFFER_SIZE 1024

int connect(int port)
{
    auto socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_file_descriptor < 0)
    {
        spdlog::error("Could not create socket!");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in socket_address;
    memset(&socket_address, 0, sizeof(socket_address));

    socket_address.sin_family = AF_INET;
    socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
    socket_address.sin_port = htons(port);

    if (bind(socket_file_descriptor, (struct sockaddr *)&socket_address, sizeof(socket_address)) < 0)
    {
        spdlog::error("Could not bind socket! Check if the port is already in use.");
        exit(EXIT_FAILURE);
    }

    if (listen(socket_file_descriptor, 10) < 0)
    {
        spdlog::error("Could not listen to socket!");
        exit(EXIT_FAILURE);
    }
    return socket_file_descriptor;
}

std::string readAll(int socket_file_descriptor)
{
    std::string wholeText;

    char buffer[BUFFER_SIZE];
    buffer[BUFFER_SIZE - 1] = '\0';
    int recv_len = 0;
    do
    {
        recv_len = read(socket_file_descriptor, buffer, sizeof(buffer) - 1);
        wholeText += std::string(buffer);
    } while (recv_len >= BUFFER_SIZE - 1);

    return wholeText;
}

int main(int argc, char const *argv[])
{
    auto socket_file_descriptor = connect(8080);

    struct sockaddr_in client_socket_address;
    socklen_t client_socket_address_size;

    CommandProcessor commandProcessor{};
    while (true)
    {
        auto client_socket_descriptor = accept(socket_file_descriptor, (struct sockaddr *)&client_socket_address, &client_socket_address_size);

        std::string wholeText = readAll(client_socket_descriptor);
        spdlog::info("Read {}", std::string(wholeText));
        commandProcessor.process(wholeText);

        close(client_socket_descriptor);
    }

    return EXIT_SUCCESS;
}
