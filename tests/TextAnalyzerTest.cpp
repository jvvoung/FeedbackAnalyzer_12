#include <gtest/gtest.h>

#include <vector>

#include "Constants.h"
#include "Feedback.h"
#include "TextAnalyzer.h"

class TextAnalyzerTest : public ::testing::Test {
protected:
    void SetUp() override { Constants::init(); }
};

// Given-When-Then
// Given: 빈 vector<Feedback>
// When:  TextAnalyzer::analyzeSentiment
// Then:  긍정=0, 중립=0, 부정=0
// test_plan: T-01
TEST_F(TextAnalyzerTest, Given_EmptyFeedbackList_When_AnalyzeSentiment_Then_AllCountsAreZero) {
    // Given: 빈 vector<Feedback>
    std::vector<Feedback> feedbacks;

    // When: TextAnalyzer::analyzeSentiment(feedbacks)
    TextAnalyzer analyzer;
    const auto result = analyzer.analyzeSentiment(feedbacks);

    // Then: 긍정=0, 중립=0, 부정=0
    EXPECT_EQ(result.at(u8"긍정"), 0);
    EXPECT_EQ(result.at(u8"중립"), 0);
    EXPECT_EQ(result.at(u8"부정"), 0);
}

// Given-When-Then
// Given: Feedback text="그냥 그래요. 특별한 감정 없음." (긍/부정 SENTIMENT_KEYWORDS 미포함)
// When:  analyzeSentiment
// Then:  감정=중립 (기본값)
// test_plan: T-03, AC-1, H-1
TEST_F(TextAnalyzerTest, Given_NeutralText_When_AnalyzeSentiment_Then_ClassifiedAsNeutral) {
    // Given: Feedback text="그냥 그래요. 특별한 감정 없음."
    std::vector<Feedback> feedbacks = {
        Feedback(u8"그냥 그래요. 특별한 감정 없음."),
    };

    // When: analyzeSentiment
    TextAnalyzer analyzer;
    const auto result = analyzer.analyzeSentiment(feedbacks);

    // Then: 감정=중립 (기본값)
    EXPECT_EQ(result.at(u8"중립"), 1);
    EXPECT_EQ(result.at(u8"긍정"), 0);
    EXPECT_EQ(result.at(u8"부정"), 0);
}

// Given-When-Then
// Given: Feedback text="품질이 별로예요." (품질 카테고리만, 감정 키워드 없음)
// When:  analyzeSentiment
// Then:  감정=중립 (카테고리≠감정 분리)
// test_plan: T-07, EX-10
TEST_F(TextAnalyzerTest, Given_CategoryKeywordOnly_When_AnalyzeSentiment_Then_ClassifiedAsNeutral) {
    // Given: Feedback text="재질이 특이합니다." (품질 카테고리만, 감정 키워드 없음)
    std::vector<Feedback> feedbacks = {
        Feedback(u8"재질이 특이합니다."),
    };

    // When: analyzeSentiment
    TextAnalyzer analyzer;
    const auto result = analyzer.analyzeSentiment(feedbacks);

    // Then: 감정=중립 (카테고리≠감정 분리)
    EXPECT_EQ(result.at(u8"긍정"), 0);
    EXPECT_EQ(result.at(u8"중립"), 1);
    EXPECT_EQ(result.at(u8"부정"), 0);
}

// Given-When-Then
// Given: 긍정·부정 SENTIMENT_KEYWORDS 동시 포함 텍스트 (각 1건 동점)
// When:  analyzeSentiment
// Then:  중립 (가중치 스코어링: positive == negative → 중립)
// test_plan: EX-09, F-06 (가중치 규칙)
TEST_F(TextAnalyzerTest, Given_MixedPositiveAndNegativeKeywords_When_AnalyzeSentiment_Then_NeutralOnTie) {
    // Given: 긍정·부정 SENTIMENT_KEYWORDS 동시 포함 텍스트
    std::vector<Feedback> feedbacks = {
        Feedback(u8"좋아요 but 나쁘기도 해요"),
    };

    // When: analyzeSentiment
    TextAnalyzer analyzer;
    const auto result = analyzer.analyzeSentiment(feedbacks);

    // Then: 동점 → 중립 (classifyWeighted)
    EXPECT_EQ(result.at(u8"중립"), 1);
    EXPECT_EQ(result.at(u8"긍정"), 0);
    EXPECT_EQ(result.at(u8"부정"), 0);
}
