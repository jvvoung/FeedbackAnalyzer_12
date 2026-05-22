#pragma once

#include <algorithm>
#include <string>
#include <vector>

namespace KeywordUtils {

inline bool containsAny(const std::string& text, const std::vector<std::string>& keywords) {
    return std::any_of(keywords.begin(), keywords.end(), [&text](const auto& kw) {
        return text.find(kw) != std::string::npos;
    });
}

}  // namespace KeywordUtils
