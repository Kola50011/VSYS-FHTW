#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <string>

#include <thread>
#include <chrono>

#include "utils/socketUtils.h"
#include "spdlog/spdlog.h"

#define BUFFER_SIZE 1024

int connect(const char *address, int port)
{
    auto socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFileDescriptor < 0)
    {
        spdlog::error("Could not create socket!");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in socketAddress;
    memset(&socketAddress, 0, sizeof(socketAddress));

    socketAddress.sin_family = AF_INET;
    inet_aton(address, &(socketAddress.sin_addr));
    socketAddress.sin_port = htons(port);

    if (connect(socketFileDescriptor, (struct sockaddr *)&socketAddress, sizeof(socketAddress)) != 0)
    {
        spdlog::error("Unable to connect to server {}!", inet_ntoa(socketAddress.sin_addr));
        exit(EXIT_FAILURE);
    }
    return socketFileDescriptor;
}

void sendRequest(int socketFileDescriptor, std::string text)
{
    socketUtils::writeAll(socketFileDescriptor, text);
}

int main(int argc, char const *argv[])
{
    auto socketFileDescriptor = connect("127.0.0.1", 8080);

    sendRequest(socketFileDescriptor, "LOL\nif19b002\ntestSubject\nThis is a great message\nwith some newlines\nand an end.\n.\n");
    socketUtils::readAll(socketFileDescriptor, BUFFER_SIZE);
    sendRequest(socketFileDescriptor, "SEND\nif19b002\ntestSubject\nThis is a great message\nwith some newlines\nand an end.\n.\n");
    socketUtils::readAll(socketFileDescriptor, BUFFER_SIZE);
    sendRequest(socketFileDescriptor, "LOGIN\ntest\npasswort\n");
    socketUtils::readAll(socketFileDescriptor, BUFFER_SIZE);
    sendRequest(socketFileDescriptor, "SEND\nif19b002\ntestSubject\nThis is a great message\nwith some newlines\nand an end.\n.\n");
    socketUtils::readAll(socketFileDescriptor, BUFFER_SIZE);

    return EXIT_SUCCESS;
}