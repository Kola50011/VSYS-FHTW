#pragma once

#include <string>

#include "libraries/uuid/uuid.h"
#include "requests/authenticatedRequest.hpp"
#include "libraries/spdlog/spdlog.h"
#include <regex>

#include "persistence/mailRepository.hpp"
#include "response.hpp"

class ReadRequest : public AuthenticatedRequest {
public:
    std::string getKeyword() override {
        return "READ";
    }

    static bool hasValidStructure(std::vector<std::string> lines) {
        if (lines.size() < 2) {
            spdlog::error("READ command has invalid amount of arguments!");
            return false;
        }

        if (uuid::is_uuid(lines.at(1))) {
            spdlog::error("Mailnumber is not a valid UUID!");
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

    std::string process(std::string requestText, Session &session) override {
        auto lines = stringUtils::split(requestText, "\n");
        auto mailId = lines.at(1);

        spdlog::info("READ Mailnumber: {} ", mailId);

        auto mailRepository = MailRepository::instance();
        if (mailRepository.mailExists(mailId)) {
            std::string ret = RESPONSE_OK;
            auto mail = mailRepository.getMail(mailId);
            spdlog::info("3");
            ret += mail.getContent();
            return ret;
        }

        return RESPONSE_ERR;
    }
};