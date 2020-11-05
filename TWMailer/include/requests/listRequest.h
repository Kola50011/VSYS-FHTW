#pragma once

#include <string>

#include "utils/stringUtils.h"
#include "requests/authenticatedRequest.h"
#include "spdlog/spdlog.h"

class ListRequest : public AuthenticatedRequest
{
public:
    std::string getKeyword()
    {
        return "LIST";
    }

    bool isValid(std::string requestText)
    {
        return requestText == "LIST\n";
    }

    std::string process(std::string requestText, Session &session)
    {
        spdlog::info("LIST");
        return "OK\n";
    }
};