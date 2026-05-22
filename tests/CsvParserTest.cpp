#include <gtest/gtest.h>

class CsvParserTest : public ::testing::Test {};

// Given-When-Then
// Given: CSV "id,comment\n1,hello" (text 컬럼 없음)
// When:  CsvParser::parse
// Then:  0건 적재 또는 parse 실패; fields[0]을 text로 사용 금지
// test_plan: T-05, AC-2, H-2
TEST_F(CsvParserTest, Given_CsvWithoutTextColumn_When_Parse_Then_ZeroFeedbacks) {
    FAIL() << "RED";  // T-05, AC-2, H-2
}

// Given-When-Then
// Given: CSV 헤더 text + 데이터 행 2건
// When:  CsvParser::parse
// Then:  Feedback 2건, 각 text 컬럼 값 보존
// test_plan: CsvParser 정상 2행 (test_plan §3.4)
TEST_F(CsvParserTest, Given_TwoDataRows_When_Parse_Then_TwoFeedbacks) {
    FAIL() << "RED";
}

// Given-When-Then
// Given: CSV 헤더만 ("text\n")
// When:  CsvParser::parse
// Then:  Feedback 0건
// test_plan: README 비정상 — 헤더만 (test_plan §3.4)
TEST_F(CsvParserTest, Given_HeaderOnlyCsv_When_Parse_Then_ZeroFeedbacks) {
    FAIL() << "RED";
}
