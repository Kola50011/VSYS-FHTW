#pragma once

#include <string>

#include "libraries/uuid/uuid.h"
#include "requests/authenticatedRequest.h"
#include "response.h"
#include "persistence/mailRepository.h"
#include "libraries/spdlog/spdlog.h"

class DeleteRequest : public AuthenticatedRequest
{
public:
    std::string getKeyword()
    {
        return "DEL";
    }

    bool hasValidStructure(std::vector<std::string> lines)
    {
        if (lines.size() < 2)
        {
            spdlog::error("DEL command has invalid amount of arguments!");
            return false;
        }

        if (uuid::is_uuid(lines.at(1)))
        {
            spdlog::error("Mailnumber is not a valid UUID!");
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
        auto mailId = lines.at(1);

        spdlog::info("DEL Mailnumber: {} ", mailId);

        auto mailRepository = MailRepository::instance();
        if (!mailRepository.mailExistsForUser(mailId, session.getUsername()))
        {
            return RESPONSE_ERR;
        }

        try
        {
            mailRepository.deleteMailForUser(mailId, session.getUsername());
            return RESPONSE_OK;
        }
        catch (const std::exception &e)
        {
            spdlog::error("Error when deleting mail {} for user {}: {}", mailId, session.getUsername(), e.what());
        }
        return RESPONSE_ERR;
    }
};