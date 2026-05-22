#include <gtest/gtest.h>

#include <vector>

#include "Constants.h"
#include "Feedback.h"
#include "Filters.h"
#include "support/TestHelpers.h"

class FiltersTest : public ::testing::Test {
protected:
    void SetUp() override { Constants::init(); }
};

// Given-When-Then
// Given: Feedback 1건 text="배송이 너무 늦어요. 화가 납니다."
// When:  analyze + filter (sentiment=부정, keyword=배송)
// Then:  집계 건수 1, 필터 결과 1건
// test_plan: T-02
TEST_F(FiltersTest, Given_NegativeDeliveryText_When_FilterNegativeAndBaeseong_Then_OneResult) {
    FAIL() << "RED";  // T-02
}

// Given-When-Then
// Given: Feedback text="그냥 그래요. 특별한 감정 없음." (긍/부정 키워드 없음)
// When:  filter sentiment=중립, keyword=전체
// Then:  Filters 결과 = TextAnalyzer 중립 집합 100% 일치
// test_plan: T-03, AC-1, H-1
TEST_F(FiltersTest, Given_NeutralText_When_FilterSentimentNeutral_Then_MatchesTextAnalyzerSet) {
    // Given: Feedback("그냥 그래요. 특별한 감정 없음.")
    std::vector<Feedback> feedbacks = {
        Feedback(u8"그냥 그래요. 특별한 감정 없음."),
    };

    // When: Filters::fil(feedbacks, u8"중립", u8"전체")
    Filters filters;
    const auto filtered = filters.fil(feedbacks, u8"중립", u8"전체");
    const auto expectedNeutral = test_support::getAnalyzerNeutralSubset(feedbacks);

    // Then: 결과 집합 == TextAnalyzer가 동일 입력에 대해 중립으로 분류한 집합
    EXPECT_EQ(filtered.size(), 1u);
    EXPECT_TRUE(test_support::feedbackSetsEqualByText(filtered, expectedNeutral));
}

// Given-When-Then
// Given: Feedback 3건 (감정·카테고리 혼합)
// When:  filter sentiment=전체, keyword=전체
// Then:  입력 전체 3건 반환
// test_plan: T-04
TEST_F(FiltersTest, Given_MixedThreeFeedbacks_When_FilterAllSentimentAllKeyword_Then_ReturnsAllThree) {
    FAIL() << "RED";  // T-04
}

// Given-When-Then
// Given: Feedback text="택배가 빨라요." (배송 main 키워드만)
// When:  filter keyword=배송, sentiment=전체
// Then:  해당 Feedback 포함
// test_plan: T-06, AC-3, H-3
TEST_F(FiltersTest, Given_MainKeywordOnly_When_FilterDelivery_Then_Included) {
    // Given: Feedback text="택배가 빨라요." (배송 main 키워드만)
    std::vector<Feedback> feedbacks = {
        Feedback(u8"택배가 빨라요."),
    };

    // When: filter keyword=배송, sentiment=전체
    Filters filters;
    const auto filtered = filters.fil(feedbacks, u8"전체", u8"배송");

    // Then: 해당 Feedback 포함
    ASSERT_EQ(filtered.size(), 1u);
    EXPECT_EQ(filtered[0].getText(), u8"택배가 빨라요.");
}
