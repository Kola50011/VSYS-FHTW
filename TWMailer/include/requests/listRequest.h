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

        std::string ret = RESPONSE_OK;
        ret += std::to_string(mails.size()) + "\n";
        for (auto &mail : mails)
        {
            ret += mail.getId() + " " + mail.getSubject() + "\n";
        }

        return ret;
    }
};