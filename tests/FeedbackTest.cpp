#include <gtest/gtest.h>

#include "Feedback.h"

#include <string>

// 정상: 텍스트 생성·getText()
TEST(FeedbackTest, Given_Text_When_CreateFeedback_Then_GetTextReturnsSame) {
    // Given
    const std::string text = u8"배송이 너무 늦어요.";

    // When
    const Feedback feedback(text);

    // Then
    EXPECT_EQ(feedback.getText(), text);
}

// 경계: 빈 문자열
TEST(FeedbackTest, Given_EmptyString_When_CreateFeedback_Then_GetTextIsEmpty) {
    // Given / When
    const Feedback feedback("");

    // Then
    EXPECT_TRUE(feedback.getText().empty());
}

// 경계: 공백만
TEST(FeedbackTest, Given_WhitespaceOnly_When_CreateFeedback_Then_PreservesWhitespace) {
    // Given
    const std::string text = "   \t  ";

    // When
    const Feedback feedback(text);

    // Then
    EXPECT_EQ(feedback.getText(), text);
}

// 경계: 멀티라인(\n 포함)
TEST(FeedbackTest, Given_MultilineText_When_CreateFeedback_Then_PreservesNewlines) {
    // Given: AC-7 연계
    const std::string text = "첫 줄\n두 번째 줄";

    // When
    const Feedback feedback(text);

    // Then
    EXPECT_EQ(feedback.getText(), text);
    EXPECT_NE(feedback.getText().find('\n'), std::string::npos);
}

// 이동/복사 기본 동작
TEST(FeedbackTest, Given_Feedback_When_CopyAndMove_Then_IndependentAndTransferred) {
    // Given
    Feedback original(u8"복사 원본");

    // When: 복사
    Feedback copied(original);
    Feedback moved(std::move(original));

    // Then
    EXPECT_EQ(copied.getText(), u8"복사 원본");
    EXPECT_EQ(moved.getText(), u8"복사 원본");
}

// 권장: 긴 UTF-8 한글 문자열
TEST(FeedbackTest, Given_LongUtf8KoreanText_When_CreateFeedback_Then_RetainsFullContent) {
    // Given
    const std::string koreanChar = u8"가";
    std::string text;
    text.reserve(512 * koreanChar.size());
    for (int i = 0; i < 512; ++i) {
        text += koreanChar;
    }

    // When
    const Feedback feedback(text);

    // Then
    EXPECT_EQ(feedback.getText().size(), text.size());
    EXPECT_EQ(feedback.getText(), text);
}

// UTF-8 한글 1건
TEST(FeedbackTest, Given_Utf8KoreanText_When_CreateFeedback_Then_GetTextMatches) {
    // Given
    const std::string text = u8"고객 피드백: 배송이 빠르고 품질도 좋습니다.";

    // When
    const Feedback feedback(text);

    // Then
    EXPECT_EQ(feedback.getText(), text);
}
