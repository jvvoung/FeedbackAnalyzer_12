#include <gtest/gtest.h>

#include "Filters.h"
#include "TextAnalyzer.h"
#include "Constants.h"
#include "support/TestHelpers.h"

class FiltersTest : public ::testing::Test {
protected:
    void SetUp() override {
        Constants::init();
        Filters::initFilterKeywords();
    }

    Filters filters;
    TextAnalyzer analyzer;
};

// T-02 — sentiment=부정 + keyword=배송 교집합
TEST_F(FiltersTest, Given_NegativeDeliveryText_When_FilterNegativeAndBaeseong_Then_OneResult) {
    // Given: Constants::SENTIMENT_KEYWORDS["부정"]("불만") + CATEGORY_KEYWORDS["배송"]["main"]("배송")
    const std::vector<Feedback> feedbacks = {
        Feedback(u8"배송이 너무 늦어요. 불만입니다."),
    };

    // When: sentiment=부정, keyword=배송
    const auto filtered = filters.fil(feedbacks, u8"부정", u8"배송");

    // Then
    ASSERT_EQ(filtered.size(), 1u);
    EXPECT_EQ(filtered[0].getText(), feedbacks[0].getText());
    const auto sentiment = analyzer.sent(filtered);
    EXPECT_EQ(sentiment.at(u8"부정"), 1);
}

// T-04 — sentinel `전체` = 해당 축 미적용
TEST_F(FiltersTest, Given_MixedThreeFeedbacks_When_FilterAllSentimentAllKeyword_Then_ReturnsAllThree) {
    // Given: 감정·카테고리 혼합 3건
    const std::vector<Feedback> feedbacks = {
        Feedback(u8"배송이 너무 늦어요. 화가 납니다."),
        Feedback(u8"품질 좋아요. 만족합니다."),
        Feedback(u8"그냥 그래요. 특별한 감정 없음."),
    };

    // When: sentiment=전체, keyword=전체
    const auto filtered = filters.fil(feedbacks, u8"전체", u8"전체");

    // Then
    EXPECT_EQ(filtered.size(), 3u);
}

// T-03, AC-1, H-1 — 중립 필터 = TextAnalyzer 중립 집합 100% 일치 (RED: failing 예상)
TEST_F(FiltersTest, Given_MixedFeedbacksIncludingNeutral_When_FilterSentimentNeutral_Then_MatchesTextAnalyzerSet) {
    // Given: T-03 canonical + Constants 전용 긍정 키워드("와우")만 포함 → Filters·TA 불일치 유발
    const std::vector<Feedback> feedbacks = {
        Feedback(u8"그냥 그래요. 특별한 감정 없음."),
        Feedback(u8"와우 정말 대단해요."),
    };
    const auto expectedNeutral = test_support::getAnalyzerNeutralSubset(feedbacks);

    // When: sentiment=중립, keyword=전체 (POST /filter 계약)
    const auto filtered = filters.fil(feedbacks, u8"중립", u8"전체");

    // Then: AC-1 — Filters 결과 = TextAnalyzer 중립 집합 (H-1 수정 전 실패)
    EXPECT_TRUE(test_support::feedbackSetsEqualByText(filtered, expectedNeutral));
    EXPECT_EQ(filtered.size(), expectedNeutral.size());
}

// T-03, AC-1, H-1 — canonical 단일 피드백 (Analyzer 중립 집합 일치 계약)
TEST_F(FiltersTest, Given_CanonicalNeutralText_When_FilterSentimentNeutral_Then_MatchesSingleAnalyzerNeutral) {
    // Given: T-03 텍스트 — 긍/부정 SENTIMENT_KEYWORDS 미포함
    const std::vector<Feedback> feedbacks = {
        Feedback(u8"그냥 그래요. 특별한 감정 없음."),
    };
    const auto expectedNeutral = test_support::getAnalyzerNeutralSubset(feedbacks);

    // When
    const auto filtered = filters.fil(feedbacks, u8"중립", u8"전체");

    // Then
    EXPECT_EQ(filtered.size(), 1u);
    EXPECT_TRUE(test_support::feedbackSetsEqualByText(filtered, expectedNeutral));
}

// T-06, AC-3, H-3 — keyword=배송 시 main 키워드 포함 피드백 포함 (RED: main skip 버그 검증)
TEST_F(FiltersTest, Given_TaekbaeMainKeywordText_When_FilterKeywordBaeseong_Then_IncludedInResult) {
    // Given: CATEGORY_KEYWORDS["배송"]["main"]의 "택배"만 포함
    const std::vector<Feedback> feedbacks = {Feedback(u8"택배가 빨라요.")};

    // When: keyword=배송, sentiment=전체
    const auto filtered = filters.fil(feedbacks, u8"전체", u8"배송");

    // Then: AC-3 — main 키워드 매칭 피드백 포함
    ASSERT_EQ(filtered.size(), 1u);
    EXPECT_NE(filtered[0].getText().find(u8"택배"), std::string::npos);
}

// T-06, AC-3, H-3 — main-only 키워드("품질") 필터 누락 (RED: Filters main skip)
TEST_F(FiltersTest, Given_QualityMainKeywordOnlyText_When_FilterKeywordQuality_Then_IncludedInResult) {
    // Given: CATEGORY_KEYWORDS["품질"]["main"]의 "품질"만 포함, sub 카테고리 미매칭
    const std::vector<Feedback> feedbacks = {Feedback(u8"품질이 좋아요.")};

    // When: keyword=품질, sentiment=전체
    const auto filtered = filters.fil(feedbacks, u8"전체", u8"품질");

    // Then: AC-3 — main 키워드 매칭 피드백 포함 (H-3 수정 전 실패)
    ASSERT_EQ(filtered.size(), 1u);
    EXPECT_NE(filtered[0].getText().find(u8"품질"), std::string::npos);
}

// AC-3 — Filters keyword=배송 결과 = TextAnalyzer main 집합 일치
TEST_F(FiltersTest, Given_MainCategoryFeedbacks_When_FilterKeywordBaeseong_Then_MatchesAnalyzerMainSet) {
    // Given: main 키워드 기준 기대 집합 (TextAnalyzer::kw 규칙)
    const std::vector<Feedback> feedbacks = {
        Feedback(u8"택배가 빨라요."),
        Feedback(u8"물류가 느립니다."),
        Feedback(u8"품질이 좋아요."),
    };
    const auto expectedMain = test_support::getMainCategoryMatches(feedbacks, u8"배송");

    // When
    const auto filtered = filters.fil(feedbacks, u8"전체", u8"배송");

    // Then: AC-3 — keyword=배송 = CATEGORY_KEYWORDS["배송"]["main"] 매칭
    EXPECT_TRUE(test_support::feedbackSetsEqualByText(filtered, expectedMain));
    EXPECT_EQ(filtered.size(), expectedMain.size());
}

// sentinel: keyword=전체 → 감정 필터만 적용
TEST_F(FiltersTest, Given_PositiveAndNegativeFeedbacks_When_FilterPositiveAllKeyword_Then_OnlyPositive) {
    // Given
    const std::vector<Feedback> feedbacks = {
        Feedback(u8"정말 좋아요."),
        Feedback(u8"정말 별로예요."),
    };

    // When: sentiment=긍정, keyword=전체 (sentinel)
    const auto filtered = filters.fil(feedbacks, u8"긍정", u8"전체");

    // Then
    ASSERT_EQ(filtered.size(), 1u);
    EXPECT_NE(filtered[0].getText().find(u8"좋아요"), std::string::npos);
}
