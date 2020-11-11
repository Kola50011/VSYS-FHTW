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

    void authenticate()
    {
        authenticated = true;
    }

    void quit()
    {
        authenticated = false;
    }

    void setUsername(std::string username)
    {
        this->username = username;
    }
};
