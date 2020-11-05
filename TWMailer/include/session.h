#pragma once

#include <string>

class Session
{
private:
    std::string username;
    bool authenticated;

public:
    Session()
    {
        this->username = "";
        authenticated = false;
    }

    Session(std::string username)
    {
        this->username = username;
        authenticated = true;
    }
    ~Session(){};

    std::string getUsername()
    {
        return username;
    }

    bool isAuthenticated()
    {
        return authenticated;
    }
};
