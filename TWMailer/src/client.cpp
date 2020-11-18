#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>

#include "utils/socketUtils.hpp"

#define BUFFER_SIZE 1024

int connect(const char *address, int port) {
    auto socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFileDescriptor < 0) {
        spdlog::error("Could not create socket!");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in socketAddress{};

    socketAddress.sin_family = AF_INET;
    inet_aton(address, &(socketAddress.sin_addr));
    socketAddress.sin_port = htons(port);

    if (connect(socketFileDescriptor, (struct sockaddr *) &socketAddress, sizeof(socketAddress)) != 0) {
        spdlog::error("Unable to connect to server {}!", inet_ntoa(socketAddress.sin_addr));
        exit(EXIT_FAILURE);
    }
    return socketFileDescriptor;
}

void sendRequest(int socketFileDescriptor, const std::string &text) {
    socketUtils::writeAll(socketFileDescriptor, text);
}

int main() {
    spdlog::set_level(spdlog::level::debug);

    auto socketFileDescriptor = connect("127.0.0.1", 8080);

    sendRequest(socketFileDescriptor, "LOGIN\ntest\npasswort\n");
    socketUtils::readAll(socketFileDescriptor, BUFFER_SIZE);
    sendRequest(socketFileDescriptor,
                "SEND\nif19b002\ntestSubject\nThis is a great message\nwith some newlines\nand an end.\n.\n");
    socketUtils::readAll(socketFileDescriptor, BUFFER_SIZE);
    sendRequest(socketFileDescriptor, "LIST\n");
    socketUtils::readAll(socketFileDescriptor, BUFFER_SIZE);
    sendRequest(socketFileDescriptor, "QUIT\n");
    socketUtils::readAll(socketFileDescriptor, BUFFER_SIZE);

    sendRequest(socketFileDescriptor, "LOGIN\nif19b002\npasswort\n");
    socketUtils::readAll(socketFileDescriptor, BUFFER_SIZE);
    sendRequest(socketFileDescriptor, "LIST\n");
    socketUtils::readAll(socketFileDescriptor, BUFFER_SIZE);
    // sendRequest(socketFileDescriptor, "DEL\n5665d682-75bb-4236-a7df-9f8cca24791e\n");
    // socketUtils::readAll(socketFileDescriptor, BUFFER_SIZE);
    // sendRequest(socketFileDescriptor, "READ\nc9150128-830c-49aa-ae0f-368ce4af0f91\n");
    // socketUtils::readAll(socketFileDescriptor, BUFFER_SIZE);

    return EXIT_SUCCESS;
}

// a138f7ab-3b86-4f7d-82a3-76f909508aff