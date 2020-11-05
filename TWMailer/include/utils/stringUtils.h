#pragma once

#include <string>
#include <vector>
#include "spdlog/spdlog.h"

namespace stringUtils
{
    std::vector<std::string> split(std::string text, std::string delimiter)
    {
        std::vector<std::string> ret;
        size_t oldPos{0};
        size_t pos{0};
        while ((pos = text.find(delimiter, oldPos)) != std::string::npos)
        {
            ret.push_back(text.substr(oldPos, pos - oldPos));
            oldPos = pos + 1;
        }
        return ret;
    }

    bool isNumber(std::string text)
    {
        return !std::regex_match(text, std::regex("^[0-9]+$"));
    }

} // namespace stringUtils