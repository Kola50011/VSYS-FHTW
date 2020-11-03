#pragma once

#include <string>

class Command
{
protected:
    Command(){};

public:
    virtual ~Command(){};
    virtual std::string getKeyword() = 0;
    virtual bool isValid(std::string requestText) = 0;
    virtual void process(std::string requestText) = 0;
};