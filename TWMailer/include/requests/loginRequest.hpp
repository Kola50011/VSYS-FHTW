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
private:
    bool disableLdap = false;

public:
    explicit LoginRequest(bool _disableLdap) : disableLdap(_disableLdap) {}

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

    std::string handleRequest(const std::string &requestText, Session &session) {
        auto lines = stringUtils::split(requestText, "\n");

        if (BanRepository::instance().isBanned(session.getIp()))
        {
            spdlog::warn("Banned user with IP {} tried to login. Dropping connection", session.getIp());
            close(session.getSocket());
        }

        std::string username = lines.at(1);
        std::string password = lines.at(2);

        spdlog::info("LOGIN {}", username);

        if (disableLdap || ldapUtils::checkUserAndPassword(username, password)) {
            session.setUsername(username);
            session.authenticate();
            return RESPONSE_OK;
        }

        BanRepository::instance().addFailed(session.getIp());
        return RESPONSE_ERR;
    }
};