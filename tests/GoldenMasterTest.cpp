#include <gtest/gtest.h>

#include <string>

#include "support/GoldenMaster.h"

#ifndef GOLDEN_DIR
#define GOLDEN_DIR "tests/golden"
#endif

namespace {

std::string domainGoldenExpectedPath() {
    return std::string(GOLDEN_DIR) + "/domain_golden_expected.txt";
}

void assertMatchesGoldenSection(const std::string& sectionId, const std::string& actual) {
    const std::string goldenFile = golden::readFile(domainGoldenExpectedPath());
    ASSERT_FALSE(goldenFile.empty()) << "Missing golden file: " << domainGoldenExpectedPath();

    const std::string expected = golden::loadGoldenSection(goldenFile, sectionId);
    ASSERT_FALSE(expected.empty()) << "Missing section [" << sectionId << "] in golden file";

    std::string diff;
    const bool matches =
        golden::compareGoldenSection(sectionId, expected, actual, &diff);
    if (!matches) {
        ADD_FAILURE() << diff;
    }
    EXPECT_TRUE(matches);
}

}  // namespace

class GoldenMasterTest : public ::testing::Test {
protected:
    void SetUp() override { Constants::init(); }
};

// GM-TC-01 (GM-D-01): T-02, TC-A-01 — negative delivery analyze maps
TEST_F(GoldenMasterTest, GM_D01_AnalyzeNegativeDelivery_MatchesGolden) {
    assertMatchesGoldenSection("GM-D-01", golden::captureGmD01Body());
}

// GM-TC-02 (GM-D-02): T-03, AC-1, TC-B-02 — neutral filter
TEST_F(GoldenMasterTest, GM_D02_FilterNeutral_MatchesGolden) {
    assertMatchesGoldenSection("GM-D-02", golden::captureGmD02Body());
}

// GM-TC-03 (GM-D-03): T-06, TC-B-05 — CSV text column parse
TEST_F(GoldenMasterTest, GM_D03_CsvParseTextColumn_MatchesGolden) {
    assertMatchesGoldenSection("GM-D-03", golden::captureGmD03Body());
}

// GM-TC-04 (GM-D-04): T-04, TC-B-03 — filter all sentiment/keyword
TEST_F(GoldenMasterTest, GM_D04_FilterAll_MatchesGolden) {
    assertMatchesGoldenSection("GM-D-04", golden::captureGmD04Body());
}

// Optional: GOLDEN_UPDATE=1 → regenerate domain_golden_expected.txt (local only, not CI)
TEST(GoldenMasterCapture, UpdateGoldenFile_WhenGoldenUpdateEnvSet) {
    Constants::init();
    if (!golden::goldenUpdateEnabled()) {
        GTEST_SKIP() << "Set GOLDEN_UPDATE=1 to regenerate domain_golden_expected.txt";
    }

    const std::string path = domainGoldenExpectedPath();
    ASSERT_TRUE(golden::updateGoldenFile(path)) << "Failed to write: " << path;

    const std::string roundTrip = golden::readFile(path);
    EXPECT_EQ(golden::buildGoldenFile(), roundTrip);
}
