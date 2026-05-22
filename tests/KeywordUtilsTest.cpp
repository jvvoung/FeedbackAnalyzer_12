#include <gtest/gtest.h>

#include <vector>

#include "support/KeywordUtils.h"

// Given-When-Then
// Given: UTF-8 한글 text, keywords 목록에 한글 키워드 포함
// When:  test_support::containsAny(text, keywords)
// Then:  부분 문자열 매칭 시 true
// test_plan: KeywordUtils containsAny 한글 (test_plan §3.5, Phase 2~3)
TEST(KeywordUtilsTest, Given_Utf8KoreanKeyword_When_ContainsAny_Then_ReturnsTrue) {
    // Given: UTF-8 한글 text, keywords 목록에 한글 키워드 포함
    const std::string text = u8"배송이 빠릅니다";
    const std::vector<std::string> keywords = {u8"배송"};

    // When/Then: 부분 문자열 매칭 시 true
    EXPECT_TRUE(test_support::containsAny(text, keywords));
}
