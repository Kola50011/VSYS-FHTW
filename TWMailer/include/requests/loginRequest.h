#pragma once

#include <string>

#include "utils/stringUtils.h"
#include "requests/request.h"
#include "libraries/spdlog/spdlog.h"
#include "session.h"
#include "response.h"
#include "persistence/banRepository.h"

class LoginRequest : public Request
{
public:
    std::string getKeyword()
    {
        return "LOGIN";
    }

    bool hasValidStructure(std::vector<std::string> lines)
    {
        if (lines.size() != 3)
        {
            spdlog::error("LOGIN command has invalid amount of arguments!");
            return false;
        }

        if (lines.at(1).length() > 8)
        {
            spdlog::error("LDAP Username is too long!");
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

    std::string handleRequest(std::string requestText, Session &session)
    {
        auto lines = stringUtils::split(requestText, "\n");

        std::string username = lines.at(1);
        std::string password = lines.at(2);

        spdlog::info("LOGIN {}", username);

        if (password != "topSecret")
        {
            BanRepository::instance().addFailed(session.getIp());
            return RESPONSE_ERR;
        }

        // TODO ADD Ldap Auth
        session.setUsername(username);
        session.authenticate();

        return RESPONSE_OK;
    }
};