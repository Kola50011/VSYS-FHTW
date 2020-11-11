#pragma once

#include <string>

#include "utils/stringUtils.h"
#include "requests/authenticatedRequest.h"
#include "libraries/spdlog/spdlog.h"

class QuitRequest : public AuthenticatedRequest
{
public:
    std::string getKeyword()
    {
        return "QUIT";
    }

    bool isValid(std::string requestText)
    {
        return requestText == "QUIT\n";
    }

    std::string process(std::string requestText, Session &session)
    {
        spdlog::info("QUIT");
        session.quit();
        return "OK\n";
    }
};