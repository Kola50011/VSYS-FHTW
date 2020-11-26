#pragma once

#include "libraries/spdlog/spdlog.h"
#include <sys/socket.h>

namespace socketUtils {
    bool isOpen(int socketFileDescriptor) {
        char buffer;
        int result = recv(socketFileDescriptor, &buffer, 1, MSG_PEEK | MSG_WAITALL);
        return (result > 0);
    }

    std::string readAll(int socketFileDescriptor, int bufferSize) {
        std::string wholeText;

        char buffer[bufferSize];
        int recvLen;
        do {
            recvLen = read(socketFileDescriptor, buffer, sizeof(buffer) - 1);
            if (recvLen < 0) { // When the connection is closed we can just treat it like we received nothing
                return "";
            }
            buffer[recvLen] = '\0';
            wholeText += std::string(buffer);
        } while (recvLen >= bufferSize - 1);

        spdlog::debug("Read {}", std::string(wholeText));

        return wholeText;
    }

    void writeAll(int socketFileDescriptor, const std::string &text, bool logging = false) {
        if (send(socketFileDescriptor, text.c_str(), strlen(text.c_str()) + 1, MSG_NOSIGNAL) < 0 && logging) {
            spdlog::error("Could not write string {}!", text);
        }
    }
} // namespace socketUtils