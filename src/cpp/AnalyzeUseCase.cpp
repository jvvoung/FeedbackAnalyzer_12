#include "AnalyzeUseCase.h"

#include "AppMessages.h"
#include "Logger.h"

AnalyzeResult AnalyzeUseCase::analyzeAll(std::vector<Feedback>& feedbacks, const std::string& text) {
    AnalyzeResult result;

    if (!text.empty()) {
        const auto start = text.find_first_not_of(" \t\r\n");
        const auto end = text.find_last_not_of(" \t\r\n");
        if (start != std::string::npos) {
            feedbacks.push_back(Feedback(text.substr(start, end - start + 1)));
        }
    }

    for (const auto& fb : feedbacks) {
        Logger::logInfo(fb.getText());
    }

    Logger::logInfo(u8"현재 " + std::to_string(feedbacks.size()) + u8"개의 피드백이 입력되었습니다.");
    result.successMessage = AppMessages::feedbackCountSuccess(feedbacks.size());

    if (!feedbacks.empty()) {
        result.sentimentResults = analyzer_.sent(feedbacks);
        result.keywordResults = analyzer_.kw(feedbacks);
        Logger::logInfo(u8"감성 분석 완료");
        Logger::logInfo(u8"키워드 분석 완료");
    }

    return result;
}
