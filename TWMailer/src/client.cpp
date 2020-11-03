#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <string>

#include "spdlog/spdlog.h"

int connect(const char *address, int port)
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
    inet_aton(address, &(socket_address.sin_addr));
    socket_address.sin_port = htons(port);

    if (connect(socket_file_descriptor, (struct sockaddr *)&socket_address, sizeof(socket_address)) != 0)
    {
        spdlog::error("Unable to connect to server {}!", inet_ntoa(socket_address.sin_addr));
        exit(EXIT_FAILURE);
    }
    return socket_file_descriptor;
}

int main(int argc, char const *argv[])
{
    auto socket_file_descriptor = connect("127.0.0.1", 8080);

    std::string text = "Hello World!";
    if (write(socket_file_descriptor, text.c_str(), strlen(text.c_str()) + 1) < 0)
    {
        spdlog::error("Could not send string {} to server!", text);
    }
    return EXIT_SUCCESS;
}