#pragma once

#include <string>
#include <vector>
#include <map>

#include "libraries/spdlog/spdlog.h"
#include "requests/request.hpp"
#include "requests/authenticatedRequest.hpp"
#include "requests/sendRequest.hpp"
#include "requests/listRequest.hpp"
#include "requests/readRequest.hpp"
#include "requests/deleteRequest.hpp"
#include "requests/loginRequest.hpp"
#include "requests/quitRequest.hpp"
#include "session.hpp"
#include "response.hpp"

class RequestProcessor {
private:
    std::map<std::string, Request *> requestMap;

public:
    explicit RequestProcessor(bool disableLdap) {
        addCommand(new SendRequest());
        addCommand(new ListRequest());
        addCommand(new ReadRequest());
        addCommand(new DeleteRequest());
        addCommand(new LoginRequest(disableLdap));
        addCommand(new QuitRequest());
    }

    ~RequestProcessor() {
        for (auto const &request : requestMap) {
            delete request.second;
        }
    }

    void addCommand(Request *request) {
        requestMap[request->getKeyword()] = request;
    }

    static std::string handleRequest(Request *request, const std::string &requestText, Session &session) {

        if (auto *authenticatedRequest = dynamic_cast<AuthenticatedRequest *>(request)) {
            return authenticatedRequest->handleRequest(requestText, session);
        }

        if (auto *loginRequest = dynamic_cast<LoginRequest *>(request)) {
            return loginRequest->handleRequest(requestText, session);
        }

        spdlog::error("Request is neither instance of AuthenticatedRequest or LoginRequest! This should NOT happen!");
        return RESPONSE_ERR;
    }

    std::string process(const std::string &requestText, Session &session) {

        auto keywordEnd = requestText.find('\n');
        if (keywordEnd == -1) {
            spdlog::error("Could not find keyword in input!");
            return RESPONSE_ERR;
        }

        std::string keyword = requestText.substr(0, keywordEnd);

        try {
            auto request = requestMap.at(keyword);

            spdlog::debug("Keyword is {}", request->getKeyword());
            if (request->isValid(requestText)) {
                spdlog::debug("Command is valid");
                return handleRequest(request, requestText, session);
            }

            spdlog::error("Command is invalid!\n{}", requestText);
            return RESPONSE_ERR;
        }
        catch (const std::out_of_range &e) {
            spdlog::error("Did not find command with keyword {}", keyword);
            return RESPONSE_ERR;
        }
    }
};