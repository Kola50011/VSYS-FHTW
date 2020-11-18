#pragma once

#include <string>
#include <utility>

class Session {
private:
    std::string ip;
    std::string username;
    bool authenticated;

public:
    Session() {
        this->username = "";
        this->ip = "";
        authenticated = false;
    }

    explicit Session(std::string ip) {
        this->ip = std::move(ip);
        authenticated = false;
    }

    ~Session() = default;

    std::string getUsername() {
        return username;
    }

    std::string getIp() {
        return ip;
    }

    bool isAuthenticated() const {
        return authenticated;
    }

    void authenticate() {
        authenticated = true;
    }

    void quit() {
        authenticated = false;
    }

    void setUsername(std::string username) {
        this->username = username;
    }
};
