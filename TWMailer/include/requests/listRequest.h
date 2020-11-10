#pragma once

#include <string>

#include "libraries/spdlog/spdlog.h"
#include "response.h"
#include "requests/authenticatedRequest.h"
#include "persistence/mailRepository.h"

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
        auto mails = MailRepository::instance().getMailsForUser(session.getUsername());

        std::string mailsString;
        int mailCount{};
        for (auto &mail : mails)
        {
            if (mail.getSender() != session.getUsername())
            {
                mailCount++;
                mailsString += mail.getId() + " " + mail.getSubject() + "\n";
            }
        }

        return RESPONSE_OK + std::to_string(mailCount) + "\n" + mailsString;
    }
};