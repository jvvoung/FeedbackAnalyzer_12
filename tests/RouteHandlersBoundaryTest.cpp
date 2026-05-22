#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <thread>

#include "AppMessages.h"
#include "Constants.h"
#include "CsvConstants.h"
#include "FeedbackSession.h"
#include "httplib.h"
#include "RouteHandlers.h"

namespace {

bool bodyContains(const std::string& body, const char* needle) {
    return body.find(needle) != std::string::npos;
}

}  // namespace

class RouteHandlersBoundaryTest : public ::testing::Test {
protected:
    void SetUp() override {
        Constants::init();
        FeedbackSession::clear();

        server_ = std::make_unique<httplib::Server>();
        handlers_ = std::make_unique<RouteHandlers>();
        handlers_->registerRoutes(*server_);

        port_ = server_->bind_to_any_port("127.0.0.1");
        ASSERT_GT(port_, 0);

        serverThread_ = std::thread([this]() { server_->listen_after_bind(); });

        client_ = std::make_unique<httplib::Client>("127.0.0.1", port_);
        client_->set_connection_timeout(2, 0);
        client_->set_read_timeout(2, 0);
    }

    void TearDown() override {
        if (server_) {
            server_->stop();
        }
        if (serverThread_.joinable()) {
            serverThread_.join();
        }
        FeedbackSession::clear();
    }

    std::unique_ptr<httplib::Server> server_;
    std::unique_ptr<RouteHandlers> handlers_;
    std::unique_ptr<httplib::Client> client_;
    int port_{0};
    std::thread serverThread_;
};

// test_plan: T-08, TC-A-02 — HTTP layer
TEST_F(RouteHandlersBoundaryTest, T08_GivenExistingFeedbacks_When_PostAnalyzeEmptyText_Then_CountUnchanged) {
    auto seed = client_->Post("/analyze", "text=기존%20피드백", "application/x-www-form-urlencoded");
    ASSERT_TRUE(seed);
    EXPECT_EQ(seed->status, 200);

    auto empty = client_->Post("/analyze", "text=", "application/x-www-form-urlencoded");
    ASSERT_TRUE(empty);
    EXPECT_EQ(empty->status, 200);
    EXPECT_TRUE(bodyContains(empty->body, u8"1개의 피드백이 입력되었습니다."));

    auto whitespace = client_->Post("/analyze", "text=%20%20", "application/x-www-form-urlencoded");
    ASSERT_TRUE(whitespace);
    EXPECT_EQ(whitespace->status, 200);
    EXPECT_TRUE(bodyContains(whitespace->body, u8"1개의 피드백이 입력되었습니다."));
}

// test_plan: T-09, TC-A-03
TEST_F(RouteHandlersBoundaryTest, T09_GivenEmptySession_When_PostFilter_Then_NoFeedbacksWarning) {
    auto res = client_->Post("/filter", "sentiment=전체&keyword=전체", "application/x-www-form-urlencoded");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);
    EXPECT_TRUE(bodyContains(res->body, AppMessages::noFeedbacksToAnalyze()));
}

// test_plan: T-10, TC-A-04
TEST_F(RouteHandlersBoundaryTest, T10_GivenMismatch_When_PostFilter_Then_NoResultsWarning) {
    auto seed = client_->Post("/analyze", "text=배송이%20너무%20늦어요.", "application/x-www-form-urlencoded");
    ASSERT_TRUE(seed);
    EXPECT_EQ(seed->status, 200);

    auto res = client_->Post("/filter", "sentiment=긍정&keyword=전체", "application/x-www-form-urlencoded");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);
    EXPECT_TRUE(bodyContains(res->body, AppMessages::filterNoResults()));
}

// test_plan: T-11, AC-7, TC-A-07 — analyze → filter → download
TEST_F(RouteHandlersBoundaryTest, T11_GivenMultilineText_When_AnalyzeFilterDownload_Then_NewlinePreservedInCsv) {
    const std::string encodedText = "text=%EC%B2%AB%20%EC%A4%84%0A%EB%91%90%20%EB%B2%88%EC%A7%B8%20%EC%A4%84";

    auto analyze = client_->Post("/analyze", encodedText, "application/x-www-form-urlencoded");
    ASSERT_TRUE(analyze);
    EXPECT_EQ(analyze->status, 200);

    auto filter = client_->Post("/filter", "sentiment=전체&keyword=전체", "application/x-www-form-urlencoded");
    ASSERT_TRUE(filter);
    EXPECT_EQ(filter->status, 200);

    auto download = client_->Get("/download");
    ASSERT_TRUE(download);
    EXPECT_EQ(download->status, 200);
    EXPECT_EQ(download->get_header_value("Content-Type"), CsvConstants::kContentType);

    const std::string expectedText = u8"첫 줄\n두 번째 줄";
    EXPECT_NE(download->body.find(expectedText), std::string::npos);
    EXPECT_NE(download->body.find(CsvConstants::kUtf8Bom), std::string::npos);
}

// test_plan: EX-04, TC-A-05, M-6 — 필터 미실행 download
TEST_F(RouteHandlersBoundaryTest, EX04_GivenNoFilter_When_GetDownload_Then_NoDownloadResultsWarning) {
    auto res = client_->Get("/download");
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);
    EXPECT_TRUE(bodyContains(res->body, AppMessages::noDownloadResults()));
}

// test_plan: EX-06, TC-A-06 — CSV text 컬럼 없음
TEST_F(RouteHandlersBoundaryTest, EX06_GivenCsvWithoutTextColumn_When_PostUpload_Then_UploadError) {
    httplib::UploadFormDataItems items = {
        {"file", "id,comment\n1,hello", "bad.csv", "text/csv"},
    };

    auto res = client_->Post("/upload", items);
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);
    EXPECT_TRUE(bodyContains(res->body, AppMessages::uploadError()));
}

// test_plan: EX-07 — 빈 CSV 파일
TEST_F(RouteHandlersBoundaryTest, EX07_GivenEmptyCsvFile_When_PostUpload_Then_UploadError) {
    httplib::UploadFormDataItems items = {
        {"file", "", "empty.csv", "text/csv"},
    };

    auto res = client_->Post("/upload", items);
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);
    EXPECT_TRUE(bodyContains(res->body, AppMessages::uploadError()));
}

// test_plan: EX-08 — CSV 헤더만
TEST_F(RouteHandlersBoundaryTest, EX08_GivenHeaderOnlyCsv_When_PostUpload_Then_SuccessZeroAdded) {
    httplib::UploadFormDataItems items = {
        {"file", "text\n", "header_only.csv", "text/csv"},
    };

    auto res = client_->Post("/upload", items);
    ASSERT_TRUE(res);
    EXPECT_EQ(res->status, 200);
    EXPECT_TRUE(bodyContains(res->body, u8"0개의 피드백이 입력되었습니다."));
}
