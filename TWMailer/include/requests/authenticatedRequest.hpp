#pragma once

#include <string>
#include <utility>

#include "libraries/spdlog/spdlog.h"
#include "requests/request.hpp"
#include "response.hpp"
#include "session.hpp"

class AuthenticatedRequest : public Request
{
private:
    virtual std::string process(std::string requestText, Session &session) = 0;

protected:
    AuthenticatedRequest()= default;

public:
    std::string handleRequest(std::string requestText, Session &session)
    {
        if (session.isAuthenticated())
        {
            return this->process(std::move(requestText), session);
        }
        else
        {
            spdlog::warn("Tried to call authenticated request without authenticated session!");
            return RESPONSE_ERR;
        }
    }
};