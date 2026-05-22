#include <gtest/gtest.h>

#include "Constants.h"
#include "SentimentClassifier.h"

class SentimentClassifierTest : public ::testing::Test {
protected:
    void SetUp() override { Constants::init(); }
};

// SC-W01: 긍정 키워드만
TEST_F(SentimentClassifierTest, SC_W01_Given_PositiveKeywordsOnly_When_ClassifyWeighted_Then_Positive) {
    const std::string text = u8"좋아요 만족";

    const SentimentScore score = SentimentClassifier::scoreWeighted(text);
    const std::string result = SentimentClassifier::classifyWeighted(text);

    EXPECT_GT(score.positive, score.negative);
    EXPECT_EQ(result, u8"긍정");
}

// SC-W02: 부정 키워드만
TEST_F(SentimentClassifierTest, SC_W02_Given_NegativeKeywordsOnly_When_ClassifyWeighted_Then_Negative) {
    const std::string text = u8"나쁘 불만";

    const SentimentScore score = SentimentClassifier::scoreWeighted(text);
    const std::string result = SentimentClassifier::classifyWeighted(text);

    EXPECT_GT(score.negative, score.positive);
    EXPECT_EQ(result, u8"부정");
}

// SC-W03: 감정 키워드 없음 → 중립 (T-03)
TEST_F(SentimentClassifierTest, SC_W03_Given_NoSentimentKeywords_When_ClassifyWeighted_Then_Neutral) {
    const std::string text = u8"그냥 그래요";

    const SentimentScore score = SentimentClassifier::scoreWeighted(text);
    const std::string result = SentimentClassifier::classifyWeighted(text);

    EXPECT_EQ(score.positive, 0);
    EXPECT_EQ(score.negative, 0);
    EXPECT_EQ(result, u8"중립");
}

// SC-W04: 긍정2 + 부정1 혼합 → 긍정 우세
TEST_F(SentimentClassifierTest, SC_W04_Given_TwoPositiveOneNegative_When_ClassifyWeighted_Then_Positive) {
    const std::string text = u8"좋아요 만족 but 나쁘";

    const SentimentScore score = SentimentClassifier::scoreWeighted(text);
    const std::string result = SentimentClassifier::classifyWeighted(text);

    EXPECT_EQ(score.positive, 2);
    EXPECT_EQ(score.negative, 1);
    EXPECT_EQ(result, u8"긍정");
}

// SC-W05: 긍정1 + 부정2 혼합 → 부정 우세
TEST_F(SentimentClassifierTest, SC_W05_Given_OnePositiveTwoNegative_When_ClassifyWeighted_Then_Negative) {
    const std::string text = u8"좋아요 but 나쁘 불만";

    const SentimentScore score = SentimentClassifier::scoreWeighted(text);
    const std::string result = SentimentClassifier::classifyWeighted(text);

    EXPECT_EQ(score.positive, 1);
    EXPECT_EQ(score.negative, 2);
    EXPECT_EQ(result, u8"부정");
}

// SC-W06: 긍정1 + 부정1 동점 → 중립 (가중치 규칙, EX-09 대체)
TEST_F(SentimentClassifierTest, SC_W06_Given_EqualPositiveAndNegative_When_ClassifyWeighted_Then_Neutral) {
    const std::string text = u8"좋아요 but 나쁘";

    const SentimentScore score = SentimentClassifier::scoreWeighted(text);
    const std::string result = SentimentClassifier::classifyWeighted(text);

    EXPECT_EQ(score.positive, 1);
    EXPECT_EQ(score.negative, 1);
    EXPECT_EQ(result, u8"중립");
}

// classify() → classifyWeighted() 위임 확인
TEST_F(SentimentClassifierTest, SC_W01b_Given_PositiveKeywordsOnly_When_Classify_Then_DelegatesToWeighted) {
    const std::string text = u8"좋아요 만족";

    EXPECT_EQ(SentimentClassifier::classify(text), SentimentClassifier::classifyWeighted(text));
    EXPECT_EQ(SentimentClassifier::classify(text), u8"긍정");
}
