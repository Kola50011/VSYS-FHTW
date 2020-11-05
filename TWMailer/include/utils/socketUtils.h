#pragma once

#include "spdlog/spdlog.h"
#include <sys/socket.h>

namespace socketUtils
{
    bool isOpen(int socketFileDescriptor)
    {
        char buffer;
        int result = recv(socketFileDescriptor, &buffer, 1, MSG_PEEK | MSG_WAITALL);
        return (result > 0);
    }

    std::string readAll(int socketFileDescriptor, int bufferSize)
    {
        std::string wholeText;

        char buffer[bufferSize];
        buffer[bufferSize - 1] = '\0';
        int recvLen = 0;
        do
        {
            recvLen = read(socketFileDescriptor, buffer, sizeof(buffer) - 1);
            wholeText += std::string(buffer);
        } while (recvLen >= bufferSize - 1);

        spdlog::info("Read {}", std::string(wholeText));

        return wholeText;
    }

    void writeAll(int socketFileDescriptor, std::string text)
    {
        if (write(socketFileDescriptor, text.c_str(), strlen(text.c_str()) + 1) < 0)
        {
            spdlog::error("Could not write string {}!", text);
        }
    }
} // namespace socketUtils