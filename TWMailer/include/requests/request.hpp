#pragma once

#include <string>

class Request {
protected:
    Request() = default;

public:
    virtual ~Request() = default;

    virtual std::string getKeyword() = 0;

    virtual bool isValid(std::string requestText) = 0;
};