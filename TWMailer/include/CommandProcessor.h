#pragma once

#include <string>
#include <vector>

#include "spdlog/spdlog.h"
#include "Command.h"
#include "SendCommand.h"

class CommandProcessor
{
private:
    std::vector<Command *> commands;

public:
    CommandProcessor()
    {
        commands.push_back(new SendCommand());
    }

    void process(std::string requestText)
    {
        auto keywordEnd = requestText.find("\n");
        if (keywordEnd == -1)
        {
            spdlog::error("Could not find keyword in input!");
            return;
        }

        std::string keyword = requestText.substr(0, keywordEnd);
        for (Command *command : commands)
        {
            if (keyword == command->getKeyword())
            {
                spdlog::info("Keyword is {}", command->getKeyword());
                if (command->isValid(requestText))
                {
                    spdlog::info("Command is valid");
                    command->process(requestText);
                }
                else
                {
                    spdlog::error("Command is invalid");
                }

                return;
            }
        }
        spdlog::error("Did not find command with keyword {}", keyword);
    }
};