#include <gtest/gtest.h>

#include "support/KeywordUtils.h"

// 정상: 키워드 목록에 부분 문자열 포함 → true
TEST(KeywordUtilsTest, Given_TextContainingKeyword_When_ContainsAny_Then_ReturnsTrue) {
    // Given
    const std::string text = u8"배송이 빠릅니다.";
    const std::vector<std::string> keywords = {u8"배송", u8"택배"};

    // When
    const bool matched = test_support::containsAny(text, keywords);

    // Then
    EXPECT_TRUE(matched);
}

// 경계: 빈 text
TEST(KeywordUtilsTest, Given_EmptyText_When_ContainsAny_Then_ReturnsFalse) {
    // Given
    const std::string text;
    const std::vector<std::string> keywords = {u8"배송"};

    // When / Then
    EXPECT_FALSE(test_support::containsAny(text, keywords));
}

// 경계: 빈 keywords
TEST(KeywordUtilsTest, Given_EmptyKeywords_When_ContainsAny_Then_ReturnsFalse) {
    // Given
    const std::string text = u8"배송이 빠릅니다.";
    const std::vector<std::string> keywords;

    // When / Then
    EXPECT_FALSE(test_support::containsAny(text, keywords));
}

// 미포함 → false
TEST(KeywordUtilsTest, Given_NoMatchingKeyword_When_ContainsAny_Then_ReturnsFalse) {
    // Given
    const std::string text = u8"가격이 저렴합니다.";
    const std::vector<std::string> keywords = {u8"배송", u8"택배"};

    // When / Then
    EXPECT_FALSE(test_support::containsAny(text, keywords));
}

// 다중 키워드 중 하나 매칭
TEST(KeywordUtilsTest, Given_MultipleKeywords_When_OneMatches_Then_ReturnsTrue) {
    // Given
    const std::string text = u8"택배가 빨라요.";
    const std::vector<std::string> keywords = {u8"배송", u8"택배", u8"물류"};

    // When
    const bool matched = test_support::containsAny(text, keywords);

    // Then
    EXPECT_TRUE(matched);
}

// UTF-8 한글 키워드 매칭
TEST(KeywordUtilsTest, Given_Utf8KoreanKeyword_When_ContainsAny_Then_ReturnsTrue) {
    // Given: Constants::CATEGORY_KEYWORDS["배송"]["main"] 계열
    const std::string text = u8"소포가 도착했습니다.";
    const std::vector<std::string> keywords = {u8"소포", u8"배달"};

    // When / Then
    EXPECT_TRUE(test_support::containsAny(text, keywords));
}
