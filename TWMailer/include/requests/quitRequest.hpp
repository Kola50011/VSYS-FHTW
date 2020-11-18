#pragma once

#include <string>

#include "utils/stringUtils.hpp"
#include "requests/authenticatedRequest.hpp"
#include "libraries/spdlog/spdlog.h"

class QuitRequest : public AuthenticatedRequest {
public:
    std::string getKeyword() override {
        return "QUIT";
    }

    bool isValid(std::string requestText) override {
        return requestText == "QUIT\n";
    }

    std::string process(std::string requestText, Session &session) override {
        spdlog::info("QUIT");
        session.quit();
        return "OK\n";
    }
};