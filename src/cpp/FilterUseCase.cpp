#include "FilterUseCase.h"

#include "FeedbackSession.h"
#include "Logger.h"

FilterResult FilterUseCase::filterAll(const std::vector<Feedback>& feedbacks,
                                      const std::string& sentiment,
                                      const std::string& keyword) {
    FilterResult result;

    if (feedbacks.empty()) {
        result.status = FilterStatus::NoFeedbacks;
        return result;
    }

    result.filtered = filters_.filter(feedbacks, sentiment, keyword);
    if (result.filtered.empty()) {
        result.status = FilterStatus::NoResults;
        return result;
    }

    FeedbackSession::setLastFiltered(result.filtered);
    result.sentimentResults = analyzer_.analyzeSentiment(result.filtered);
    result.keywordResults = analyzer_.analyzeKeywords(result.filtered);
    Logger::logInfo(u8"필터링 결과: " + std::to_string(result.filtered.size()) + u8"개의 피드백");
    result.status = FilterStatus::Success;
    return result;
}
