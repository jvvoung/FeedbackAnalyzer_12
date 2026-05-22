#include <gtest/gtest.h>

#include <cstdlib>
#include <string>

#include "golden/DomainGoldenCapture.h"

#ifndef GOLDEN_DIR
#define GOLDEN_DIR "tests/golden"
#endif

namespace {

std::string domainGoldenExpectedPath() {
    return std::string(GOLDEN_DIR) + "/domain_golden_expected.txt";
}

}  // namespace

class DomainGoldenMasterTest : public ::testing::Test {
protected:
    void SetUp() override { Constants::init(); }
};

// Given-When-Then
// Given: test_plan §2·§4 Domain 시나리오 GM-D-01~04 입력
// When:  DomainGoldenCapture 직렬화 (타임스탬프·랜덤값 제외)
// Then:  tests/golden/domain_golden_expected.txt 와 바이트 단위 일치
TEST_F(DomainGoldenMasterTest, Given_DomainScenarios_When_Capture_Then_MatchesGoldenFile) {
    const std::string actual = golden::captureAllDomainGolden();
    const std::string expected = golden::readFile(domainGoldenExpectedPath());

    ASSERT_FALSE(expected.empty()) << "Missing golden file: " << domainGoldenExpectedPath();
    EXPECT_EQ(expected, actual) << "Domain golden mismatch. "
                               << "Set UPDATE_GOLDEN=1 and run GoldenMasterCapture to refresh.";
}

// 1회 실행용 캡처 — UPDATE_GOLDEN=1 일 때만 기준 파일 갱신
//   ctest -R GoldenMasterCapture --test-dir build
//   UPDATE_GOLDEN=1 ctest -R GoldenMasterCapture --test-dir build
TEST(DomainGoldenMasterCapture, GoldenMasterCapture) {
    Constants::init();
    const char* updateFlag = std::getenv("UPDATE_GOLDEN");
    if (updateFlag == nullptr || std::string(updateFlag) != "1") {
        GTEST_SKIP() << "Set UPDATE_GOLDEN=1 to regenerate domain_golden_expected.txt";
    }

    const std::string captured = golden::captureAllDomainGolden();
    const std::string path = domainGoldenExpectedPath();
    ASSERT_TRUE(golden::writeFile(path, captured))
        << "Failed to write golden file: " << path;

    const std::string roundTrip = golden::readFile(path);
    EXPECT_EQ(captured, roundTrip);
}
