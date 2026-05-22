#pragma once

#include <string>
#include <vector>

namespace test_support {

// Phase 2~3 containsAny 통합 전 테스트용 헬퍼 (src/cpp 미수정)
inline bool containsAny(const std::string& text, const std::vector<std::string>& keywords) {
    for (const auto& kw : keywords) {
        if (text.find(kw) != std::string::npos) {
            return true;
        }
    }
    return false;
}

}  // namespace test_support
