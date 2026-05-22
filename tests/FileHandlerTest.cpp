#include <gtest/gtest.h>

#include <chrono>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

#include "CsvConstants.h"
#include "CsvExporter.h"
#include "Feedback.h"
#include "FileHandler.h"

namespace {

std::string uniqueTempCsvPath() {
    const auto tick = std::chrono::steady_clock::now().time_since_epoch().count();
    return "filehandler_fh_test_" + std::to_string(tick) + ".csv";
}

std::string readFileBytes(const std::string& path) {
    std::ifstream ifs(path, std::ios::binary);
    return std::string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
}

void removeIfExists(const std::string& path) {
    std::remove(path.c_str());
}

}  // namespace

class FileHandlerTest : public ::testing::Test {
protected:
    void TearDown() override { removeIfExists(tempPath_); }

    std::string tempPath_ = uniqueTempCsvPath();
};

// FH-01: 정상 1건 저장 — 파일 존재, BOM+text\n+본문, return true
TEST_F(FileHandlerTest, FH01_Given_OneFeedback_When_SaveToCsv_Then_FileHasBomHeaderAndBody) {
    const std::vector<Feedback> data{Feedback(u8"배송이 빨라요")};

    const bool saved = FileHandler::saveToCsv(data, tempPath_);

    EXPECT_TRUE(saved);
    const std::string content = readFileBytes(tempPath_);
    EXPECT_EQ(content, CsvExporter::exportFilteredFeedbacks(data));
}

// FH-02: 빈 vector 저장 — BOM+text\n만, return true
TEST_F(FileHandlerTest, FH02_Given_EmptyVector_When_SaveToCsv_Then_HeaderOnly) {
    const std::vector<Feedback> data;

    const bool saved = FileHandler::saveToCsv(data, tempPath_);

    EXPECT_TRUE(saved);
    const std::string content = readFileBytes(tempPath_);
    EXPECT_EQ(content, CsvExporter::exportFilteredFeedbacks(data));
    EXPECT_EQ(content, std::string(CsvConstants::kUtf8Bom) + CsvConstants::kTextColumnHeader);
}

// FH-03: 개행 포함 text (AC-7) — \n 유지 저장
TEST_F(FileHandlerTest, FH03_Given_MultilineText_When_SaveToCsv_Then_PreservesNewlines) {
    const std::string multiline = u8"첫 줄\n두 번째 줄";
    const std::vector<Feedback> data{Feedback(multiline)};

    const bool saved = FileHandler::saveToCsv(data, tempPath_);

    EXPECT_TRUE(saved);
    const std::string content = readFileBytes(tempPath_);
    EXPECT_EQ(content, CsvExporter::exportFilteredFeedbacks(data));
    ASSERT_FALSE(data.empty());
    EXPECT_NE(content.find(data[0].getText()), std::string::npos);
    EXPECT_NE(content.find(u8"첫 줄"), std::string::npos);
    EXPECT_NE(content.find(u8"두 번째 줄"), std::string::npos);
}

// FH-04: 잘못된 path (쓰기 불가) — return false
TEST_F(FileHandlerTest, FH04_Given_InvalidPath_When_SaveToCsv_Then_ReturnsFalse) {
    const std::vector<Feedback> data{Feedback("hello")};
#if defined(_WIN32)
    const std::string badPath = "C:\\invalid<>?\\file.csv";
#else
    const std::string badPath = "/proc/nonexistent/file.csv";
#endif

    EXPECT_FALSE(FileHandler::saveToCsv(data, badPath));
}

// FH-05: 저장 후 바이트 검증 — 선두 3바이트 = EF BB BF
TEST_F(FileHandlerTest, FH05_Given_OneFeedback_When_SaveToCsv_Then_LeadingBytesAreUtf8Bom) {
    const std::vector<Feedback> data{Feedback(u8"테스트")};

    ASSERT_TRUE(FileHandler::saveToCsv(data, tempPath_));

    const std::string content = readFileBytes(tempPath_);
    ASSERT_GE(content.size(), 3u);
    EXPECT_EQ(static_cast<std::uint8_t>(content[0]), 0xEF);
    EXPECT_EQ(static_cast<std::uint8_t>(content[1]), 0xBB);
    EXPECT_EQ(static_cast<std::uint8_t>(content[2]), 0xBF);
}
