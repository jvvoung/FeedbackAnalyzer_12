#pragma once

#include <string>

#include "Constants.h"
#include "KeywordUtils.h"
#include "SentimentClassifier.h"

class FeedbackClassifier {
public:
    static std::string classifySentiment(const std::string& text) {
        return SentimentClassifier::classify(text);
    }

    static bool matchCategory(const std::string& text, const std::string& category) {
        if (!Constants::CATEGORY_KEYWORDS.count(category)) {
            return false;
        }
        const auto& catMap = Constants::CATEGORY_KEYWORDS.at(category);
        if (!catMap.count("main")) {
            return false;
        }
        return KeywordUtils::containsAny(text, catMap.at("main"));
    }
};
