#pragma once

#include <string>

#include "libraries/spdlog/spdlog.h"
#include "requests/request.h"
#include "response.h"
#include "session.h"

class AuthenticatedRequest : public Request
{
private:
    virtual std::string process(std::string requestText, Session &session) = 0;

protected:
    AuthenticatedRequest(){};

public:
    virtual bool isValid(std::string requestText) = 0;

    std::string handleRequest(std::string requestText, Session &session)
    {
        if (session.isAuthenticated())
        {
            return this->process(requestText, session);
        }
        else
        {
            spdlog::warn("Tried to call authenticated request without authenticated session!");
            return RESPONSE_ERR;
        }
    }
};