#include "FilterUseCase.h"

#include "Logger.h"

FilterResult FilterUseCase::filterAll(const std::vector<Feedback>& feedbacks,
                                      const std::string& sentiment,
                                      const std::string& keyword) {
    FilterResult result;

    if (feedbacks.empty()) {
        result.status = FilterStatus::NoFeedbacks;
        return result;
    }

    result.filtered = filters_.fil(feedbacks, sentiment, keyword);
    if (result.filtered.empty()) {
        result.status = FilterStatus::NoResults;
        return result;
    }

    filteredSnapshot_ = result.filtered;
    result.sentimentResults = analyzer_.sent(result.filtered);
    result.keywordResults = analyzer_.kw(result.filtered);
    Logger::logInfo(u8"필터링 결과: " + std::to_string(result.filtered.size()) + u8"개의 피드백");
    result.status = FilterStatus::Success;
    return result;
}
