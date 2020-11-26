#pragma once

#include <string>
#include <utility>

class Session {
private:
    std::string ip;
    int socket;
    std::string username;
    bool authenticated;

public:
    Session() {
        this->username = "";
        this->socket = -1;
        this->ip = "";
        authenticated = false;
    }

    explicit Session(std::string ip, int socket) {
        this->ip = std::move(ip);
        this->socket = socket;
        authenticated = false;
    }

    ~Session() = default;

    std::string getUsername() {
        return username;
    }

    std::string getIp() {
        return ip;
    }

    int getSocket() const {
        return socket;
    }

    bool isAuthenticated() const {
        return authenticated;
    }

    void authenticate() {
        authenticated = true;
    }

    void setUsername(std::string _username) {
        this->username = std::move(_username);
    }
};
