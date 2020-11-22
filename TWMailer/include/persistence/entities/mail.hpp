#pragma once

#include <string>
#include <utility>
#include <vector>

#include "libraries/json/json.hpp"

namespace entities {
    class Mail {
    private:
        std::string id;
        std::string sender;
        std::vector<std::string> receivers;
        std::string subject;
        std::string content;

    public:
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Mail, id, sender, receivers, subject, content)

        Mail() = default;

        Mail(std::string _sender, std::vector<std::string> _receivers, std::string _subject, std::string _content)
                : sender(std::move(_sender)), receivers(std::move(_receivers)), subject(std::move(_subject)),
                  content(std::move(_content)) {}

        ~Mail() = default;

        std::string getSender() {
            return sender;
        }

        std::vector<std::string> getReceivers() {
            return receivers;
        }

        std::string getSubject() {
            return subject;
        }

        std::string getId() {
            return id;
        }

        void setId(std::string _id) {
            id = std::move(_id);
        }

        std::string toString() {
            std::ostringstream concatReceivers;
            std::copy(receivers.begin(), receivers.end(), std::ostream_iterator<std::string>(concatReceivers, " "));
            return std::string(sender).append("\n").append(concatReceivers.str()).append("\n").append(subject).append(
                    "\n").append(content);
        }
    };
} // namespace entities
