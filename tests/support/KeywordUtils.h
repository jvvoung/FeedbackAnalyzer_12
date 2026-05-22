#pragma once

#include <string>
#include <vector>

#include "KeywordUtils.h"

namespace test_support {

inline bool containsAny(const std::string& text, const std::vector<std::string>& keywords) {
    return KeywordUtils::containsAny(text, keywords);
}

}  // namespace test_support
