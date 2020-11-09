#pragma once

#include <string>

#include "libraries/spdlog/spdlog.h"
#include "utils/stringUtils.h"
#include "requests/authenticatedRequest.h"
#include "persistence/mailRepository.h"
#include "persistence/entities/mail.h"

class SendRequest : public AuthenticatedRequest
{
public:
    std::string getKeyword()
    {
        return "SEND";
    }

    bool hasValidStructure(std::vector<std::string> lines)
    {
        if (lines.size() < 5)
        {
            spdlog::error("SEND command has invalid amount of arguments!");
            return false;
        }

        if (lines.at(1).length() > 8)
        {
            spdlog::error("Receiver name is too long!");
            return false;
        }

        if (lines.at(2).length() > 80)
        {
            spdlog::error("Subject is too long!");
            return false;
        }

        if (lines.back() != ".")
        {
            spdlog::error("Command was not terminated correctly!");
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

    std::string process(std::string requestText, Session &session)
    {
        auto lines = stringUtils::split(requestText, "\n");

        // Remove terminator element
        lines.pop_back();

        auto receiver = lines.at(1);
        auto subject = lines.at(2);

        std::string content;
        for (size_t i = 3; i < lines.size(); i++)
        {
            content += lines.at(i) + "\n";
        }

        spdlog::info("SEND Receiver: {} \n Subject: {} \n Content: {}", receiver, subject, content);

        saveMail(
            session.getUsername(),
            {receiver},
            subject,
            content);

        return "OK\n";
    }

    void saveMail(std::string sender, std::set<std::string> receivers, std::string subject, std::string content)
    {
        entities::Mail mail{sender, receivers, subject, content};
        MailRepository::instance().addMail(mail);
    }
};