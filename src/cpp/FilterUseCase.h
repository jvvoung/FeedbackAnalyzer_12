#pragma once

#include <map>
#include <string>
#include <vector>

#include "Feedback.h"
#include "Filters.h"
#include "TextAnalyzer.h"

enum class FilterStatus {
    Success,
    NoResults,
    NoFeedbacks
};

struct FilterResult {
    FilterStatus status = FilterStatus::NoFeedbacks;
    std::vector<Feedback> filtered;
    std::map<std::string, int> sentimentResults;
    std::map<std::string, int> keywordResults;
};

class FilterUseCase {
public:
    FilterUseCase(TextAnalyzer& analyzer, Filters& filters, std::vector<Feedback>& filteredSnapshot)
        : analyzer_(analyzer), filters_(filters), filteredSnapshot_(filteredSnapshot) {}

    FilterResult filterAll(const std::vector<Feedback>& feedbacks,
                           const std::string& sentiment,
                           const std::string& keyword);

private:
    TextAnalyzer& analyzer_;
    Filters& filters_;
    std::vector<Feedback>& filteredSnapshot_;
};
