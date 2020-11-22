#pragma once

#include <string>
#include <vector>

namespace stringUtils {
    std::vector<std::string> split(const std::string &str, const std::string &delim, bool withEmptyElements = true) {
        std::vector<std::string> tokens;
        size_t prev = 0, pos;
        do {
            pos = str.find(delim, prev);
            if (pos == std::string::npos) {
                pos = str.length();
            }
            std::string token = str.substr(prev, pos - prev);
            if (!token.empty() || withEmptyElements) {
                tokens.push_back(token);
            }
            prev = pos + delim.length();
        } while (pos < str.length() && prev < str.length());
        return tokens;
    }
} // namespace stringUtils