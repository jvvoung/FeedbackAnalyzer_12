#pragma once

#include <string>
#include <vector>

#include "Feedback.h"
#include "FeedbackClassifier.h"

namespace test_support {

inline std::string classifySentimentWithAnalyzerRules(const std::string& text) {
    return FeedbackClassifier::classifySentiment(text);
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

inline std::vector<Feedback> getMainCategoryMatches(const std::vector<Feedback>& feedbacks,
                                                      const std::string& category) {
    std::vector<Feedback> result;
    for (const auto& feedback : feedbacks) {
        if (FeedbackClassifier::matchCategory(feedback.getText(), category)) {
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
