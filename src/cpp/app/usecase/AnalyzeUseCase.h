#pragma once

#include <map>
#include <string>
#include <vector>

#include "Feedback.h"
#include "TextAnalyzer.h"

struct AnalyzeResult {
    std::string successMessage;
    std::map<std::string, int> sentimentResults;
    std::map<std::string, int> keywordResults;
};

class AnalyzeUseCase {
public:
    explicit AnalyzeUseCase(TextAnalyzer& analyzer) : analyzer_(analyzer) {}

    AnalyzeResult analyzeAll(std::vector<Feedback>& feedbacks, const std::string& text);

private:
    TextAnalyzer& analyzer_;
};
