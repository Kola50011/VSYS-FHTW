#pragma once

#include <string>

class Session
{
private:
    std::string ip;
    std::string username;
    bool authenticated;

public:
    Session()
    {
        this->username = "";
        this->ip = "";
        authenticated = false;
    }

    Session(std::string ip)
    {
        this->ip = ip;
        authenticated = false;
    }

    Session(std::string ip, std::string username)
    {
        this->ip = ip;
        this->username = username;
        authenticated = true;
    }
    ~Session(){};

    std::string getUsername()
    {
        return username;
    }

    std::string getIp()
    {
        return ip;
    }

    bool isAuthenticated()
    {
        return authenticated;
    }
};
