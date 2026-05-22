#include "RouteHandlers.h"

#include "AnalyzeUseCase.h"
#include "AppMessages.h"
#include "CsvExporter.h"
#include "CsvParser.h"
#include "FilterUseCase.h"
#include "FormParser.h"
#include "HtmlPageRenderer.h"
#include "Logger.h"
#include "FeedbackSession.h"

namespace {

void setHtmlResponse(httplib::Response& res, const PageViewModel& viewModel) {
    res.set_content(HtmlPageRenderer::render(viewModel), "text/html; charset=UTF-8");
}

}  // namespace

void RouteHandlers::registerRoutes(httplib::Server& server) {
    server.Get("/", [this](const httplib::Request& req, httplib::Response& res) {
        handleGetRoot(req, res);
    });
    server.Post("/analyze", [this](const httplib::Request& req, httplib::Response& res) {
        handlePostAnalyze(req, res);
    });
    server.Post("/upload", [this](const httplib::Request& req, httplib::Response& res) {
        handlePostUpload(req, res);
    });
    server.Post("/filter", [this](const httplib::Request& req, httplib::Response& res) {
        handlePostFilter(req, res);
    });
    server.Get("/download", [this](const httplib::Request& req, httplib::Response& res) {
        handleGetDownload(req, res);
    });
}

void RouteHandlers::handleGetRoot(const httplib::Request&, httplib::Response& res) {
    setHtmlResponse(res, {AppMessages::analyzerStart(), "", "", {}, {}});
}

void RouteHandlers::handlePostAnalyze(const httplib::Request& req, httplib::Response& res) {
    try {
        auto& feedbacks = FeedbackSession::getCurrent();
        auto params = FormParser::parse(req.body);
        AnalyzeUseCase analyzeUseCase(textAnalyzer_);
        const AnalyzeResult result = analyzeUseCase.analyzeAll(feedbacks, params["text"]);
        setHtmlResponse(res, {result.successMessage, "", "", result.sentimentResults, result.keywordResults});
    } catch (const std::exception& e) {
        Logger::logError(std::string(u8"오류 발생: ") + e.what());
        setHtmlResponse(res, {"", "", AppMessages::processingError(), {}, {}});
    }
}

void RouteHandlers::handlePostUpload(const httplib::Request& req, httplib::Response& res) {
    try {
        auto& feedbacks = FeedbackSession::getCurrent();
        if (req.form.has_file("file")) {
            const auto file = req.form.get_file("file");
            if (file.content.empty()) {
                setHtmlResponse(res, {"", "", AppMessages::uploadError(), {}, {}});
                return;
            }

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
        setHtmlResponse(res, {AppMessages::feedbackCountSuccess(feedbacks.size()), "", "", {}, {}});
    } catch (const std::exception& e) {
        Logger::logError(std::string(u8"파일 업로드 오류: ") + e.what());
        setHtmlResponse(res, {"", "", AppMessages::uploadError(), {}, {}});
    }
}

void RouteHandlers::handlePostFilter(const httplib::Request& req, httplib::Response& res) {
    try {
        const auto& feedbacks = FeedbackSession::getCurrent();
        auto params = FormParser::parse(req.body);
        FilterUseCase filterUseCase(textAnalyzer_, filters_);
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
}

void RouteHandlers::handleGetDownload(const httplib::Request&, httplib::Response& res) {
    const auto& filtered = FeedbackSession::getLastFiltered();
    if (filtered.empty()) {
        Logger::logWarning(AppMessages::noDownloadResults());
        setHtmlResponse(res, {"", AppMessages::noDownloadResults(), "", {}, {}});
        return;
    }
    res.set_header("Content-Disposition", CsvExporter::contentDisposition());
    res.set_content(CsvExporter::exportFilteredFeedbacks(filtered), CsvExporter::contentType());
}
