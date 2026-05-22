#include <gtest/gtest.h>

#include "support/CsvParser.h"

class CsvParserTest : public ::testing::Test {
protected:
    CsvParser parser;
};

// 정상: text 헤더 + 1 데이터 행
TEST_F(CsvParserTest, Given_ValidTwoLineCsv_When_Parse_Then_OneFeedback) {
    // Given
    const std::string csv = "text\n배송이 너무 늦어요. 화가 납니다.\n";

    // When
    const CsvParseResult result = parser.parse(csv);

    // Then
    ASSERT_TRUE(result.success);
    ASSERT_EQ(result.feedbacks.size(), 1u);
    EXPECT_EQ(result.feedbacks[0].getText(), u8"배송이 너무 늦어요. 화가 납니다.");
}

// 경계: 헤더만 → 0건
TEST_F(CsvParserTest, Given_HeaderOnlyCsv_When_Parse_Then_ZeroFeedbacks) {
    // Given
    const std::string csv = "text\n";

    // When
    const CsvParseResult result = parser.parse(csv);

    // Then
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.feedbacks.size(), 0u);
}

// T-05, AC-2, H-2 — text 컬럼 없음 → 0건 (RED: stub이 main.cpp와 동일하게 fields[0] fallback)
// As-Is: feedbacks.size()==1 ("1"). To-Be: empty 또는 parse 실패 — fields[0]을 text로 사용 금지
TEST_F(CsvParserTest, Given_NoTextColumnCsv_When_Parse_Then_ZeroFeedbacks) {
    // Given: PRD §5.3 — text 컬럼 필수; id,comment 형식
    const std::string csv = "id,comment\n1,hello\n";

    // When
    const CsvParseResult result = parser.parse(csv);

    // Then: AC-2 — fields[0]("1")을 text로 사용 금지, 0건 또는 parse 실패
    EXPECT_TRUE(result.feedbacks.empty());
}

// PRD §5.3 — 추가 컬럼 허용·무시
TEST_F(CsvParserTest, Given_ExtraColumnsCsv_When_Parse_Then_UsesTextColumnOnly) {
    // Given
    const std::string csv = "id,text,rating\n1,배송이 빨라요,5\n";

    // When
    const CsvParseResult result = parser.parse(csv);

    // Then
    ASSERT_EQ(result.feedbacks.size(), 1u);
    EXPECT_EQ(result.feedbacks[0].getText(), u8"배송이 빨라요");
}

// AC-7 연계 — UTF-8 한글·개행 포함 text (RFC 4180 quoted field)
TEST_F(CsvParserTest, Given_Utf8MultilineTextInCsv_When_Parse_Then_PreservesNewlines) {
    // Given
    const std::string csv = "text\n\"첫 줄\n두 번째 줄\"\n";

    // When
    const CsvParseResult result = parser.parse(csv);

    // Then
    ASSERT_EQ(result.feedbacks.size(), 1u);
    EXPECT_EQ(result.feedbacks[0].getText(), "첫 줄\n두 번째 줄");
}

// 정상: 2행 데이터
TEST_F(CsvParserTest, Given_TwoDataRows_When_Parse_Then_TwoFeedbacks) {
    // Given
    const std::string csv =
        "text\n"
        "배송이 너무 늦어요.\n"
        "품질 좋아요.\n";

    // When
    const CsvParseResult result = parser.parse(csv);

    // Then
    ASSERT_EQ(result.feedbacks.size(), 2u);
    EXPECT_EQ(result.feedbacks[0].getText(), u8"배송이 너무 늦어요.");
    EXPECT_EQ(result.feedbacks[1].getText(), u8"품질 좋아요.");
}
