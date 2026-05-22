#include <gtest/gtest.h>

#include "support/KeywordUtils.h"

// Given-When-Then
// Given: UTF-8 한글 text, keywords 목록에 한글 키워드 포함
// When:  test_support::containsAny(text, keywords)
// Then:  부분 문자열 매칭 시 true
// test_plan: KeywordUtils containsAny 한글 (test_plan §3.5, Phase 2~3)
TEST(KeywordUtilsTest, Given_Utf8KoreanKeyword_When_ContainsAny_Then_ReturnsTrue) {
    FAIL() << "RED";  // KeywordUtils containsAny 한글
}
