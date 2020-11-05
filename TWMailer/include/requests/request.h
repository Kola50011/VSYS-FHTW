#pragma once

#include <string>

class Request
{
protected:
    Request(){};

public:
    virtual ~Request(){};
    virtual std::string getKeyword() = 0;
    virtual bool isValid(std::string requestText) = 0;
};