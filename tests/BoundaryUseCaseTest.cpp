#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "AnalyzeUseCase.h"
#include "AppMessages.h"
#include "Constants.h"
#include "CsvExporter.h"
#include "Feedback.h"
#include "FeedbackSession.h"
#include "FilterUseCase.h"
#include "Filters.h"
#include "TextAnalyzer.h"

class BoundaryUseCaseTest : public ::testing::Test {
protected:
    void SetUp() override {
        Constants::init();
        FeedbackSession::clear();
    }

    void TearDown() override { FeedbackSession::clear(); }
};

// Given-When-Then
// Given: Session에 기존 N건
// When:  AnalyzeUseCase::analyzeAll with text="" or whitespace only
// Then:  피드백 미추가, 건수 N 유지
// test_plan: T-08, TC-A-02
TEST_F(BoundaryUseCaseTest, T08_GivenExistingFeedbacks_When_AnalyzeEmptyOrWhitespaceText_Then_CountUnchanged) {
    std::vector<Feedback> feedbacks = {Feedback(u8"기존 피드백")};
    const std::size_t before = feedbacks.size();

    TextAnalyzer analyzer;
    AnalyzeUseCase useCase(analyzer);

    useCase.analyzeAll(feedbacks, "");
    EXPECT_EQ(feedbacks.size(), before);

    useCase.analyzeAll(feedbacks, "   \t\n");
    EXPECT_EQ(feedbacks.size(), before);
    EXPECT_EQ(useCase.analyzeAll(feedbacks, "   ").successMessage,
              AppMessages::feedbackCountSuccess(before));
}

// Given-When-Then
// Given: Session 비어 있음
// When:  FilterUseCase::filterAll (sentiment=전체, keyword=전체)
// Then:  NoFeedbacks status
// test_plan: T-09, TC-A-03
TEST_F(BoundaryUseCaseTest, T09_GivenEmptySession_When_FilterAll_Then_NoFeedbacksStatus) {
    const std::vector<Feedback> empty;
    TextAnalyzer analyzer;
    Filters filters;
    FilterUseCase useCase(analyzer, filters);

    const FilterResult result = useCase.filterAll(empty, u8"전체", u8"전체");

    EXPECT_EQ(result.status, FilterStatus::NoFeedbacks);
    EXPECT_TRUE(result.filtered.empty());
}

// Given-When-Then
// Given: Feedback 1건, 필터 조건 불일치 (sentiment=긍정, 본문 부정)
// When:  FilterUseCase::filterAll
// Then:  NoResults status
// test_plan: T-10, TC-A-04
TEST_F(BoundaryUseCaseTest, T10_GivenMismatch_When_FilterPositiveSentiment_Then_NoResultsStatus) {
    const std::vector<Feedback> feedbacks = {Feedback(u8"배송이 너무 늦어요. 화가 납니다.")};
    TextAnalyzer analyzer;
    Filters filters;
    FilterUseCase useCase(analyzer, filters);

    const FilterResult result = useCase.filterAll(feedbacks, u8"긍정", u8"전체");

    EXPECT_EQ(result.status, FilterStatus::NoResults);
    EXPECT_TRUE(result.filtered.empty());
}

// Given-When-Then
// Given: text="첫 줄\n두 번째 줄"
// When:  analyze → filter (전체/전체) → CsvExporter
// Then:  Session·CSV 본문에 리터럴 \n 유지
// test_plan: T-11, AC-7, TC-A-07
TEST_F(BoundaryUseCaseTest, T11_GivenMultilineText_When_AnalyzeFilterExport_Then_NewlinePreserved) {
    const std::string multiline = u8"첫 줄\n두 번째 줄";
    std::vector<Feedback> feedbacks;

    TextAnalyzer analyzer;
    AnalyzeUseCase analyzeUseCase(analyzer);
    analyzeUseCase.analyzeAll(feedbacks, multiline);

    ASSERT_EQ(feedbacks.size(), 1u);
    EXPECT_EQ(feedbacks[0].getText(), multiline);

    Filters filters;
    FilterUseCase filterUseCase(analyzer, filters);
    const FilterResult filterResult = filterUseCase.filterAll(feedbacks, u8"전체", u8"전체");
    ASSERT_EQ(filterResult.status, FilterStatus::Success);
    EXPECT_EQ(FeedbackSession::getLastFiltered().size(), 1u);
    EXPECT_EQ(FeedbackSession::getLastFiltered()[0].getText(), multiline);

    const std::string csv = CsvExporter::exportFilteredFeedbacks(filterResult.filtered);
    EXPECT_NE(csv.find(multiline), std::string::npos);
}
