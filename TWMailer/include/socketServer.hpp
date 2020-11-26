#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cerrno>
#include <thread>

#include "libraries/spdlog/spdlog.h"
#include "utils/socketUtils.hpp"
#include "requestProcessor.hpp"
#include "session.hpp"
#include "persistence/banRepository.hpp"

#define BUFFER_SIZE 1024
#define CONNECTION_QUEUE_SIZE 10

class SocketServer {
private:
    int socketFileDescriptor{};
    bool running = true;

    void openSocket(int port) {
        socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
        if (socketFileDescriptor < 0) {
            spdlog::error("Could not create socket!");
            exit(EXIT_FAILURE);
        }

        struct sockaddr_in socketAddress{};

        socketAddress.sin_family = AF_INET;
        socketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
        socketAddress.sin_port = htons(port);

        if (bind(socketFileDescriptor, (struct sockaddr *) &socketAddress, sizeof(socketAddress)) < 0) {
            spdlog::error("Could not bind socket! Check if the port is already in use.");
            exit(EXIT_FAILURE);
        }

        if (listen(socketFileDescriptor, CONNECTION_QUEUE_SIZE) < 0) {
            spdlog::error("Could not listen to socket!");
            exit(EXIT_FAILURE);
        }
    }

    static void
    handleClient(int clientSocketFileDescriptor, RequestProcessor &requestProcessor, const std::string &ip) {
        spdlog::info("Incoming connection with ID {} and IP {}", clientSocketFileDescriptor, ip);

        if (BanRepository::instance().isBanned(ip)) {
            spdlog::warn("Banned user with IP {} tried to connect. Dropping connection", ip);
            close(clientSocketFileDescriptor);
            return;
        }

        Session session{ip, clientSocketFileDescriptor};
        while (socketUtils::isOpen(clientSocketFileDescriptor)) {
            std::string wholeText = socketUtils::readAll(clientSocketFileDescriptor, BUFFER_SIZE);
            socketUtils::writeAll(clientSocketFileDescriptor, requestProcessor.process(wholeText, session), true);
        }

        spdlog::info("Closed connection with ID {}", clientSocketFileDescriptor);
        close(clientSocketFileDescriptor);
    }

public:
    explicit SocketServer(int port) {
        openSocket(port);
    };

    ~SocketServer() {
        close(socketFileDescriptor);
        running = false;
    }

    void open(bool disableLdap) {
        struct sockaddr_in clientSocketAddress{};
        socklen_t clientSocketAddressSize = sizeof(clientSocketAddress);

        RequestProcessor requestProcessor(disableLdap);
        while (running) {
            auto clientSocketDescriptor = accept(socketFileDescriptor, (struct sockaddr *) &clientSocketAddress,
                                                 &clientSocketAddressSize);
            struct in_addr ipAddr = clientSocketAddress.sin_addr;
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &ipAddr, ip, INET_ADDRSTRLEN);

            // detach automatically releases all resources once the thread is finished
            std::thread(SocketServer::handleClient, clientSocketDescriptor, std::ref(requestProcessor), ip).detach();
        }
        running = true;
    };
};