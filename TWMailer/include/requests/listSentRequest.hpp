#pragma once

#include <string>

#include "libraries/spdlog/spdlog.h"
#include "response.hpp"
#include "requests/authenticatedRequest.hpp"
#include "persistence/mailRepository.hpp"

class ListSentRequest : public AuthenticatedRequest {
public:
    std::string getKeyword() override {
        return "LIST_SENT";
    }

    bool isValid(std::string requestText) override {
        return requestText == "LIST_SENT\n";
    }

    std::string process(std::string requestText, Session &session) override {
        spdlog::info("LIST_SENT");
        auto mails = MailRepository::instance().getSentMailsForUser(session.getUsername());

        std::string mailsString;
        int mailCount{};
        for (auto &mail : mails) {
            mailCount++;
            mailsString += mail.getId() + " " + mail.getSubject() + "\n";
        }

        return RESPONSE_OK + std::to_string(mailCount) + "\n" + mailsString;
    }
};