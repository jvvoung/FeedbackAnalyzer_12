#include <gtest/gtest.h>

#include "Feedback.h"

// Given-When-Then
// Given: text="첫 줄\n두 번째 줄"
// When:  Feedback 생성 후 getText()
// Then:  개행(\n) 유지
// test_plan: T-11, AC-7
TEST(FeedbackTest, Given_MultilineText_When_CreateFeedback_Then_PreservesNewlines) {
    FAIL() << "RED";  // T-11, AC-7
}

// Given-When-Then
// Given: 빈 문자열 text=""
// When:  Feedback 생성 후 getText()
// Then:  빈 문자열 반환
// test_plan: Feedback 경계 (test_plan §3.5)
TEST(FeedbackTest, Given_EmptyString_When_CreateFeedback_Then_GetTextIsEmpty) {
    // Given: 빈 문자열 text=""
    Feedback feedback("");

    // When/Then: getText() == ""
    EXPECT_TRUE(feedback.getText().empty());
}
