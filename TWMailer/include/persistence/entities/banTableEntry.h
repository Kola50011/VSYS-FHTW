#pragma once

#include <chrono>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>

#include "libraries/json/json.hpp"

#define TIME_STRING "%Y-%m-%dT%H:%M:%S.%z%Z"

namespace entities
{
    class BanTableEntry
    {
    private:
        std::string lastInteraction;
        int tries;

        std::string currentTime()
        {
            auto in_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            std::stringstream stream;
            stream << std::put_time(std::localtime(&in_time_t), TIME_STRING);
            return stream.str();
        }

    public:
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(BanTableEntry, lastInteraction, tries)
        BanTableEntry()
        {
            tries = 0;
            updateLastInteraction();
        };
        ~BanTableEntry(){};

        std::chrono::system_clock::time_point getLastInteraction()
        {
            std::tm time{};
            std::stringstream stream(lastInteraction);
            stream >> std::get_time(&time, TIME_STRING);
            return std::chrono::system_clock::from_time_t(std::mktime(&time));
        }

        int getTries()
        {
            return tries;
        }

        void updateLastInteraction()
        {
            lastInteraction = currentTime();
        }

        void setTries(int _tries)
        {
            tries = _tries;
        }
    };
} // namespace entities
