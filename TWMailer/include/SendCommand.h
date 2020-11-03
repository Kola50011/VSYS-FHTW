#pragma once

#include <string>

#include "StringUtils.h"
#include "Command.h"
#include "spdlog/spdlog.h"

class SendCommand : public Command
{
public:
    std::string getKeyword()
    {
        return "SEND";
    }

    bool hasValidStructure(std::vector<std::string> lines)
    {
        if (lines.size() < 6)
        {
            spdlog::error("SEND command has invalid amount of arguments!");
            return false;
        }

        if (lines.at(1).length() > 8 || lines.at(2).length() > 8)
        {
            spdlog::error("Sender / receiver name is too long!");
            return false;
        }

        if (lines.at(3).length() > 80)
        {
            spdlog::error("Subject is too long!");
            return false;
        }

        if (lines.back() != ".")
        {
            spdlog::error("Command was not terminated correctly!");
            return false;
        }
        return true;
    }

    bool isValid(std::string requestText)
    {
        auto lines = stringUtils::split(requestText, "\n");

        if (!hasValidStructure(lines))
        {
            return false;
        }

        return true;
    }

    void process(std::string requestText)
    {
        auto lines = stringUtils::split(requestText, "\n");

        // Remove terminator element
        lines.pop_back();

        auto sender = lines.at(1);
        auto receiver = lines.at(2);
        auto subject = lines.at(3);

        std::string content;
        for (size_t i = 4; i < lines.size(); i++)
        {
            content += lines.at(i) + "\n";
        }

        spdlog::info("{} {} {} {}", sender, receiver, subject, content);
    }
};