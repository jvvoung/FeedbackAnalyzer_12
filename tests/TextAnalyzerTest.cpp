#include <gtest/gtest.h>

#include "TextAnalyzer.h"
#include "Constants.h"
#include "support/KeywordUtils.h"
#include "support/TestHelpers.h"

class TextAnalyzerTest : public ::testing::Test {
protected:
    void SetUp() override {
        Constants::init();
    }

    TextAnalyzer analyzer;
};

// Given-When-Then
// 판정 순서: Constants::SENTIMENT_KEYWORDS["긍정"] → ["부정"] → 기본 "중립"
TEST_F(TextAnalyzerTest, Given_PositiveKeywordText_When_AnalyzeSentiment_Then_PositiveCountIsOne) {
    // Given: Constants::SENTIMENT_KEYWORDS["긍정"] 키워드("좋아요", "만족" 등) 포함 텍스트
    const std::vector<Feedback> feedbacks = {Feedback(u8"품질 좋아요. 만족합니다.")};

    // When
    const auto result = analyzer.sent(feedbacks);

    // Then
    EXPECT_EQ(result.at(u8"긍정"), 1);
    EXPECT_EQ(result.at(u8"중립"), 0);
    EXPECT_EQ(result.at(u8"부정"), 0);
}

TEST_F(TextAnalyzerTest, Given_NegativeKeywordText_When_AnalyzeSentiment_Then_NegativeCountIsOne) {
    // Given: Constants::SENTIMENT_KEYWORDS["부정"] 키워드("불만" 등) 포함 — T-02 계열
    const std::vector<Feedback> feedbacks = {Feedback(u8"배송이 너무 늦어요. 불만입니다.")};

    // When
    const auto result = analyzer.sent(feedbacks);

    // Then
    EXPECT_EQ(result.at(u8"부정"), 1);
    EXPECT_EQ(result.at(u8"긍정"), 0);
    EXPECT_EQ(result.at(u8"중립"), 0);
}

// T-03, AC-1 — TextAnalyzer 중립: 긍/부정 키워드 미매칭 시 기본값
TEST_F(TextAnalyzerTest, Given_NeutralTextWithoutSentimentKeywords_When_AnalyzeSentiment_Then_NeutralCountIsOne) {
    // Given: Constants::SENTIMENT_KEYWORDS["긍정"/"부정"] 모두 미포함
    const std::vector<Feedback> feedbacks = {Feedback(u8"그냥 그래요. 특별한 감정 없음.")};

    // When
    const auto result = analyzer.sent(feedbacks);

    // Then
    EXPECT_EQ(result.at(u8"중립"), 1);
    EXPECT_EQ(result.at(u8"긍정"), 0);
    EXPECT_EQ(result.at(u8"부정"), 0);
}

// T-01 — 빈 Feedback 목록
TEST_F(TextAnalyzerTest, Given_EmptyFeedbackList_When_AnalyzeSentiment_Then_AllCountsAreZero) {
    // Given
    const std::vector<Feedback> feedbacks;

    // When
    const auto result = analyzer.sent(feedbacks);

    // Then
    EXPECT_EQ(result.at(u8"긍정"), 0);
    EXPECT_EQ(result.at(u8"중립"), 0);
    EXPECT_EQ(result.at(u8"부정"), 0);
}

// T-07, EX-10 — 카테고리 키워드만, 감정 키워드 없음 → 중립
// Invariant: 카테고리(품질)≠감정(3분류); "별로"는 SENTIMENT_KEYWORDS["부정"]에 있어 본 테스트에서 제외
TEST_F(TextAnalyzerTest, Given_CategoryKeywordOnly_When_AnalyzeSentiment_Then_ClassifiedAsNeutral) {
    // Given: CATEGORY_KEYWORDS["품질"]["main"]("품질")만 포함, SENTIMENT_KEYWORDS 미포함
    const std::vector<Feedback> feedbacks = {Feedback(u8"품질이 형편없어요.")};

    // When
    const auto result = analyzer.sent(feedbacks);

    // Then: 감정=중립 (카테고리≠감정 분리)
    EXPECT_EQ(result.at(u8"중립"), 1);
    EXPECT_EQ(test_support::classifySentimentWithAnalyzerRules(feedbacks[0].getText()), u8"중립");
}

// EX-09 — 긍정·부정 키워드 동시 포함 → 긍정 우선
TEST_F(TextAnalyzerTest, Given_MixedPositiveAndNegativeKeywords_When_AnalyzeSentiment_Then_PositiveFirst) {
    // Given: Constants::SENTIMENT_KEYWORDS["긍정"]("좋아요") + ["부정"]("불만") 동시 포함
    const std::vector<Feedback> feedbacks = {Feedback(u8"좋아요 그런데 불만도 있어요.")};

    // When
    const auto result = analyzer.sent(feedbacks);

    // Then: 판정 순서 긍정 → 부정 → 중립
    EXPECT_EQ(result.at(u8"긍정"), 1);
    EXPECT_EQ(result.at(u8"부정"), 0);
}

// T-06 — CATEGORY_KEYWORDS["배송"]["main"]("택배", "배송" 등) 집계
TEST_F(TextAnalyzerTest, Given_DeliveryMainKeywordText_When_AnalyzeKeywords_Then_BaeseongCountIsOne) {
    // Given: main 키워드 "택배" 포함 (Constants::CATEGORY_KEYWORDS["배송"]["main"])
    const std::vector<Feedback> feedbacks = {Feedback(u8"택배가 빨라요.")};

    // When
    const auto result = analyzer.kw(feedbacks);

    // Then
    EXPECT_EQ(result.at(u8"배송"), 1);
    EXPECT_EQ(result.at(u8"품질"), 0);
}
