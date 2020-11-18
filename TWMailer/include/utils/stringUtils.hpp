#pragma once

#include <string>
#include <vector>
#include <regex>
#include "libraries/spdlog/spdlog.h"

namespace stringUtils
{
    std::vector<std::string> split(const std::string& text, const std::string& delimiter)
    {
        std::vector<std::string> ret;
        size_t oldPos{0};
        size_t pos;
        while ((pos = text.find(delimiter, oldPos)) != std::string::npos)
        {
            ret.push_back(text.substr(oldPos, pos - oldPos));
            oldPos = pos + 1;
        }
        return ret;
    }

} // namespace stringUtils