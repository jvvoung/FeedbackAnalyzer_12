#include "httplib.h"
#include "Feedback.h"
#include "Constants.h"
#include "AppState.h"
#include "Session.h"
#include "TextAnalyzer.h"
#include "Filters.h"
#include "AnalyzeUseCase.h"
#include "FilterUseCase.h"
#include "FormParser.h"
#include "CsvParser.h"
#include "AppMessages.h"
#include "HtmlPageRenderer.h"
#include "CsvExporter.h"
#include "FileHandler.h"
#include "Logger.h"
#include <string>

static TextAnalyzer textAnalyzer;
static Filters filters;
static FileHandler fileHandler;

static void setHtmlResponse(httplib::Response& res, const PageViewModel& viewModel) {
    res.set_content(HtmlPageRenderer::render(viewModel), "text/html; charset=UTF-8");
}

int main() {
    Constants::init();

    httplib::Server svr;

    // GET /
    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        Session::initSessionStateUgly();
        setHtmlResponse(res, {AppMessages::analyzerStart(), "", "", {}, {}});
    });

    // POST /analyze
    svr.Post("/analyze", [](const httplib::Request& req, httplib::Response& res) {
        try {
            auto& feedbacks = Session::getCurrentFeedbacks();
            auto params = FormParser::parse(req.body);
            AnalyzeUseCase analyzeUseCase(textAnalyzer);
            const AnalyzeResult result = analyzeUseCase.analyzeAll(feedbacks, params["text"]);
            setHtmlResponse(res, {result.successMessage, "", "", result.sentimentResults, result.keywordResults});
        } catch (const std::exception& e) {
            Logger::logError(std::string(u8"오류 발생: ") + e.what());
            setHtmlResponse(res, {"", "", AppMessages::processingError(), {}, {}});
        }
    });

    // POST /upload
    svr.Post("/upload", [](const httplib::Request& req, httplib::Response& res) {
        try {
            auto& feedbacks = Session::getCurrentFeedbacks();
            if (req.form.has_file("file")) {
                const auto file = req.form.get_file("file");
                if (!file.content.empty()) {
                    CsvParser parser;
                    const CsvParseResult parseResult = parser.parse(file.content);
                    if (!parseResult.hasTextColumn) {
                        Logger::logError(u8"CSV에 text 컬럼이 없습니다.");
                        setHtmlResponse(res, {"", "", AppMessages::uploadError(), {}, {}});
                        return;
                    }
                    for (const auto& feedback : parseResult.feedbacks) {
                        feedbacks.push_back(feedback);
                    }
                    Logger::logInfo(u8"파일이 성공적으로 업로드되었습니다.");
                }
            }
            setHtmlResponse(res, {AppMessages::feedbackCountSuccess(feedbacks.size()), "", "", {}, {}});
        } catch (const std::exception& e) {
            Logger::logError(std::string(u8"파일 업로드 오류: ") + e.what());
            setHtmlResponse(res, {"", "", AppMessages::uploadError(), {}, {}});
        }
    });

    // POST /filter
    svr.Post("/filter", [](const httplib::Request& req, httplib::Response& res) {
        try {
            const auto& feedbacks = Session::getCurrentFeedbacks();
            auto params = FormParser::parse(req.body);
            FilterUseCase filterUseCase(textAnalyzer, filters);
            const FilterResult result =
                filterUseCase.filterAll(feedbacks, params["sentiment"], params["keyword"]);

            if (result.status == FilterStatus::Success) {
                setHtmlResponse(res, {"", "", "", result.sentimentResults, result.keywordResults});
            } else if (result.status == FilterStatus::NoResults) {
                Logger::logWarning(AppMessages::filterNoResults());
                setHtmlResponse(res, {"", AppMessages::filterNoResults(), "", {}, {}});
            } else {
                Logger::logWarning(AppMessages::noFeedbacksToAnalyze());
                setHtmlResponse(res, {"", AppMessages::noFeedbacksToAnalyze(), "", {}, {}});
            }
        } catch (const std::exception& e) {
            Logger::logError(std::string(u8"오류 발생: ") + e.what());
            setHtmlResponse(res, {"", "", AppMessages::processingError(), {}, {}});
        }
    });

    // GET /download
    svr.Get("/download", [](const httplib::Request&, httplib::Response& res) {
        const auto& filtered = AppState::lastFilteredFeedbacksConst();
        if (filtered.empty()) {
            Logger::logWarning(AppMessages::noDownloadResults());
            setHtmlResponse(res, {"", AppMessages::noDownloadResults(), "", {}, {}});
            return;
        }
        res.set_header("Content-Disposition", CsvExporter::contentDisposition());
        res.set_content(CsvExporter::exportFilteredFeedbacks(filtered), CsvExporter::contentType());
    });

    Logger::logInfo(u8"서버가 http://localhost:8080 에서 시작됩니다.");
    svr.listen("0.0.0.0", 8080);

    return 0;
}
