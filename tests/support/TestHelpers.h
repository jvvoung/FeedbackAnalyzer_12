#pragma once

#include <string>
#include <vector>

#include "Feedback.h"
#include "Constants.h"
#include "KeywordUtils.h"
#include "SentimentClassifier.h"

namespace test_support {

inline std::string classifySentimentWithAnalyzerRules(const std::string& text) {
    return SentimentClassifier::classify(text);
}

inline std::vector<Feedback> getAnalyzerNeutralSubset(const std::vector<Feedback>& feedbacks) {
    std::vector<Feedback> result;
    for (const auto& feedback : feedbacks) {
        if (classifySentimentWithAnalyzerRules(feedback.getText()) == u8"중립") {
            result.push_back(feedback);
        }
    }
    return result;
}

// TextAnalyzer::kw와 동일: CATEGORY_KEYWORDS[cat]["main"] 기준
inline std::vector<Feedback> getMainCategoryMatches(const std::vector<Feedback>& feedbacks,
                                                      const std::string& category) {
    std::vector<Feedback> result;
    if (!Constants::CATEGORY_KEYWORDS.count(category)) {
        return result;
    }
    const auto& mainKeywords = Constants::CATEGORY_KEYWORDS.at(category).at("main");
    for (const auto& feedback : feedbacks) {
        if (KeywordUtils::containsAny(feedback.getText(), mainKeywords)) {
            result.push_back(feedback);
        }
    }
    return result;
}

inline bool feedbackSetsEqualByText(const std::vector<Feedback>& a, const std::vector<Feedback>& b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (std::size_t i = 0; i < a.size(); ++i) {
        if (a[i].getText() != b[i].getText()) {
            return false;
        }
    }
    return true;
}

}  // namespace test_support
