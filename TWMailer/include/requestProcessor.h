#pragma once

#include <string>
#include <vector>
#include <map>

#include "libraries/spdlog/spdlog.h"
#include "requests/request.h"
#include "requests/authenticatedRequest.h"
#include "requests/sendRequest.h"
#include "requests/listRequest.h"
#include "requests/readRequest.h"
#include "requests/deleteRequest.h"
#include "requests/loginRequest.h"
#include "requests/quitRequest.h"
#include "session.h"
#include "response.h"

class RequestProcessor
{
private:
    LoginRequest loginRequest;
    std::map<std::string, Request *> requestMap;

public:
    RequestProcessor()
    {
        addCommand(new SendRequest());
        addCommand(new ListRequest());
        addCommand(new ReadRequest());
        addCommand(new DeleteRequest());
        addCommand(new LoginRequest());
        addCommand(new QuitRequest());
    }

    ~RequestProcessor()
    {
        for (auto const &request : requestMap)
        {
            delete request.second;
        }
    }

    void addCommand(Request *request)
    {
        requestMap[request->getKeyword()] = request;
    }

    std::string handleRequest(Request *request, std::string requestText, Session &session)
    {

        if (AuthenticatedRequest *authenticatedRequest = dynamic_cast<AuthenticatedRequest *>(request))
        {
            return authenticatedRequest->handleRequest(requestText, session);
        }

        if (LoginRequest *loginRequest = dynamic_cast<LoginRequest *>(request))
        {
            spdlog::info("instanceof LoginRequest");
            return loginRequest->handleRequest(requestText, session);
        }

        spdlog::error("request is not instanceof!");
        return RESPONSE_ERR;
    }

    std::string process(std::string requestText, Session &session)
    {

        auto keywordEnd = requestText.find("\n");
        if (keywordEnd == -1)
        {
            spdlog::error("Could not find keyword in input!");
            return RESPONSE_ERR;
        }

        std::string keyword = requestText.substr(0, keywordEnd);

        try
        {
            auto request = requestMap.at(keyword);

            spdlog::info("Keyword is {}", request->getKeyword());
            if (request->isValid(requestText))
            {
                spdlog::info("Command is valid");
                return handleRequest(request, requestText, session);
            }

            spdlog::error("Command is invalid");
            return RESPONSE_ERR;
        }
        catch (const std::out_of_range &e)
        {
            spdlog::error("Did not find command with keyword {}", keyword);
            return RESPONSE_ERR;
        }
    }
};