#include <gtest/gtest.h>

class TextAnalyzerTest : public ::testing::Test {};

// Given-When-Then
// Given: 빈 vector<Feedback>
// When:  TextAnalyzer::analyzeSentiment (sent)
// Then:  긍정=0, 중립=0, 부정=0
// test_plan: T-01
TEST_F(TextAnalyzerTest, Given_EmptyFeedbackList_When_AnalyzeSentiment_Then_AllCountsAreZero) {
    FAIL() << "RED";  // T-01
}

// Given-When-Then
// Given: Feedback text="그냥 그래요. 특별한 감정 없음." (긍/부정 SENTIMENT_KEYWORDS 미포함)
// When:  analyzeSentiment
// Then:  감정=중립 (기본값)
// test_plan: T-03, AC-1, H-1
TEST_F(TextAnalyzerTest, Given_NeutralText_When_AnalyzeSentiment_Then_ClassifiedAsNeutral) {
    FAIL() << "RED";  // T-03, AC-1, H-1
}

// Given-When-Then
// Given: Feedback text="품질이 별로예요." (품질 카테고리만, 감정 키워드 없음)
// When:  analyzeSentiment
// Then:  감정=중립 (카테고리≠감정 분리)
// test_plan: T-07, EX-10
TEST_F(TextAnalyzerTest, Given_CategoryKeywordOnly_When_AnalyzeSentiment_Then_ClassifiedAsNeutral) {
    FAIL() << "RED";  // T-07, EX-10
}

// Given-When-Then
// Given: 긍정·부정 SENTIMENT_KEYWORDS 동시 포함 텍스트
// When:  analyzeSentiment
// Then:  긍정 우선 (판정 순서: 긍정 → 부정 → 중립)
// test_plan: EX-09, F-06
TEST_F(TextAnalyzerTest, Given_MixedPositiveAndNegativeKeywords_When_AnalyzeSentiment_Then_PositiveFirst) {
    FAIL() << "RED";  // EX-09, F-06
}
