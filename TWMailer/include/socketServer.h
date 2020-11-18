#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <thread>

#include "libraries/spdlog/spdlog.h"
#include "utils/socketUtils.h"
#include "requestProcessor.h"
#include "session.h"
#include "persistence/banRepository.h"

#define BUFFER_SIZE 1024

class SocketServer
{
private:
    int socketFileDescriptor;

    void openSocket(int port)
    {
        socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
        if (socketFileDescriptor < 0)
        {
            spdlog::error("Could not create socket!");
            exit(EXIT_FAILURE);
        }

        struct sockaddr_in socketAddress;
        memset(&socketAddress, 0, sizeof(socketAddress));

        socketAddress.sin_family = AF_INET;
        socketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
        socketAddress.sin_port = htons(port);

        if (bind(socketFileDescriptor, (struct sockaddr *)&socketAddress, sizeof(socketAddress)) < 0)
        {
            spdlog::error("Could not bind socket! Check if the port is already in use.");
            exit(EXIT_FAILURE);
        }

        if (listen(socketFileDescriptor, 10) < 0)
        {
            spdlog::error("Could not listen to socket!");
            exit(EXIT_FAILURE);
        }
    }

    void handleClient(int socketFileDescriptor, RequestProcessor &requestProcessor, std::string ip)
    {
        spdlog::info("Incoming connection with ID {} and IP {}", socketFileDescriptor, ip);

        if (BanRepository::instance().isBanned(ip))
        {
            spdlog::warn("Banned user with IP {} tried to connect. Dropping connection", ip);
            close(socketFileDescriptor);
            return;
        }

        Session session{ip};
        while (socketUtils::isOpen(socketFileDescriptor))
        {
            std::string wholeText = socketUtils::readAll(socketFileDescriptor, BUFFER_SIZE);
            socketUtils::writeAll(socketFileDescriptor, requestProcessor.process(wholeText, session));
        }

        spdlog::info("Closed connection with ID {}", socketFileDescriptor);
        close(socketFileDescriptor);
    }

public:
    SocketServer(int port)
    {
        openSocket(port);
    };

    ~SocketServer()
    {
        close(socketFileDescriptor);
    }

    void open()
    {
        struct sockaddr_in clientSocketAddress;
        socklen_t clientSocketAddressSize = sizeof(clientSocketAddress);

        RequestProcessor requestProcessor{};
        while (true)
        {
            auto clientSocketDescriptor = accept(socketFileDescriptor, (struct sockaddr *)&clientSocketAddress, &clientSocketAddressSize);
            struct in_addr ipAddr = clientSocketAddress.sin_addr;
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &ipAddr, ip, INET_ADDRSTRLEN);

            // detach automatically releases all ressources once the thread is finished
            std::thread(&SocketServer::handleClient, this, clientSocketDescriptor, std::ref(requestProcessor), ip).detach();
        }
    };
};