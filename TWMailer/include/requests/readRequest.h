#pragma once

#include <string>

#include "utils/stringUtils.h"
#include "requests/authenticatedRequest.h"
#include "spdlog/spdlog.h"
#include <regex>

class ReadRequest : public AuthenticatedRequest
{
public:
    std::string getKeyword()
    {
        return "READ";
    }

    bool hasValidStructure(std::vector<std::string> lines)
    {
        if (lines.size() < 2)
        {
            spdlog::error("READ command has invalid amount of arguments!");
            return false;
        }

        bool non_numberic = !std::regex_match(lines.at(1), std::regex("^[0-9]+$"));
        if (non_numberic)
        {
            spdlog::error("Mailnumber contains invalid charaters!");
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

    std::string process(std::string requestText, Session &session)
    {
        auto lines = stringUtils::split(requestText, "\n");

        auto mail_number = stoi(lines.at(1));

        spdlog::info("READ Mailnumber: {} ", mail_number);

        return "OK\n";
    }
};