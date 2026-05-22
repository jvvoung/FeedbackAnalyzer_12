#include <gtest/gtest.h>

#include "CsvParser.h"

// Given-When-Then
// Given: CSV "id,comment\n1,hello" (text 컬럼 없음)
// When:  CsvParser::parse
// Then:  0건 적재 또는 parse 실패; fields[0]을 text로 사용 금지
// test_plan: T-05, AC-2, H-2
TEST(CsvParserTest, Given_CsvWithoutTextColumn_When_Parse_Then_ZeroFeedbacks) {
    // Given: CSV "id,comment\n1,hello" (text 컬럼 없음)
    const std::string csv = "id,comment\n1,hello";

    // When: CsvParser::parse
    CsvParser parser;
    const auto result = parser.parse(csv);

    // Then: 0건; fields[0] fallback 금지
    EXPECT_TRUE(result.feedbacks.empty());
}

// Given-When-Then
// Given: CSV 헤더 text + 데이터 행 2건
// When:  CsvParser::parse
// Then:  Feedback 2건, 각 text 컬럼 값 보존
// test_plan: CsvParser 정상 2행 (test_plan §3.4)
TEST(CsvParserTest, Given_TwoDataRows_When_Parse_Then_TwoFeedbacks) {
    // Given: CSV 헤더 text + 데이터 행 2건
    const std::string csv = "text\n첫 번째\n두 번째";

    // When: CsvParser::parse
    CsvParser parser;
    const auto result = parser.parse(csv);

    // Then: Feedback 2건, text 컬럼 값 보존
    ASSERT_EQ(result.feedbacks.size(), 2u);
    EXPECT_EQ(result.feedbacks[0].getText(), u8"첫 번째");
    EXPECT_EQ(result.feedbacks[1].getText(), u8"두 번째");
}

// Given-When-Then
// Given: CSV 헤더만 ("text\n")
// When:  CsvParser::parse
// Then:  Feedback 0건
// test_plan: README 비정상 — 헤더만 (test_plan §3.4)
TEST(CsvParserTest, Given_HeaderOnlyCsv_When_Parse_Then_ZeroFeedbacks) {
    // Given: CSV 헤더만
    const std::string csv = "text\n";

    // When: CsvParser::parse
    CsvParser parser;
    const auto result = parser.parse(csv);

    // Then: Feedback 0건
    EXPECT_TRUE(result.feedbacks.empty());
}
