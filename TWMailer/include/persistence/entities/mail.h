#pragma once

#include <string>
#include <set>

#include "libraries/json/json.hpp"

namespace entities
{
    class Mail
    {
    private:
        std::string id;
        std::string sender;
        std::set<std::string> receivers;
        std::string subject;
        std::string content;

    public:
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Mail, id, sender, receivers, subject, content)
        Mail(){};
        Mail(std::string _sender, std::set<std::string> _receivers, std::string _subject, std::string _content)
        {
            sender = _sender;
            receivers = _receivers;
            subject = _subject;
            content = _content;
        }
        ~Mail(){};

        std::string getSender()
        {
            return sender;
        }

        std::set<std::string> getReceivers()
        {
            return receivers;
        }

        std::string getSubject()
        {
            return subject;
        }

        std::string getContent()
        {
            return content;
        }

        std::string getId()
        {
            return id;
        }

        void setId(std::string _id)
        {
            id = _id;
        }
    };
} // namespace entities
