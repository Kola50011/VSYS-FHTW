#pragma once

#include <string>

#include "libraries/spdlog/spdlog.h"
#include "response.hpp"
#include "requests/authenticatedRequest.hpp"
#include "persistence/mailRepository.hpp"

class ListRequest : public AuthenticatedRequest {
public:
    std::string getKeyword() override {
        return "LIST";
    }

    bool isValid(std::string requestText) override {
        return requestText == "LIST\n";
    }

    std::string process(std::string requestText, Session &session) override {
        spdlog::info("LIST");
        auto mails = MailRepository::instance().getMailsForUser(session.getUsername());

        std::string mailsString;
        int mailCount{};
        for (auto &mail : mails) {
            if (std::find(mail.getReceivers().begin(), mail.getReceivers().end(), session.getUsername()) !=
                mail.getReceivers().end()) {
                mailCount++;
                mailsString += mail.getId() + " " + mail.getSubject() + "\n";
            }
        }

        return RESPONSE_OK + std::to_string(mailCount) + "\n" + mailsString;
    }
};