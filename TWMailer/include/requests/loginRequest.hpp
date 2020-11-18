#pragma once

#include <string>

#include "utils/stringUtils.hpp"
#include "requests/request.hpp"
#include "libraries/spdlog/spdlog.h"
#include "session.hpp"
#include "response.hpp"
#include "persistence/banRepository.hpp"
#include "utils/ldapUtils.hpp"

class LoginRequest : public Request {
public:
    std::string getKeyword() override {
        return "LOGIN";
    }

    static bool hasValidStructure(std::vector<std::string> lines) {
        if (lines.size() != 3) {
            spdlog::error("LOGIN command has invalid amount of arguments!");
            return false;
        }

        if (lines.at(1).length() > 8) {
            spdlog::error("LDAP Username is too long!");
            return false;
        }

        return true;
    }

    bool isValid(std::string requestText) override {
        auto lines = stringUtils::split(requestText, "\n");

        if (!hasValidStructure(lines)) {
            return false;
        }
        return true;
    }

    static std::string handleRequest(const std::string& requestText, Session &session) {
        auto lines = stringUtils::split(requestText, "\n");

        std::string username = lines.at(1);
        std::string password = lines.at(2);

        spdlog::info("LOGIN {}", username);

        //TODO LDAP!!!
        if (password == "badPW") {
            BanRepository::instance().addFailed(session.getIp());
            return RESPONSE_ERR;
        }

        // TODO ADD Ldap Auth
        session.setUsername(username);
        session.authenticate();

        return RESPONSE_OK;
    }
};