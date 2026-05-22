#pragma once

#include <string>

#include "Constants.h"
#include "KeywordUtils.h"

class SentimentClassifier {
public:
    static std::string classify(const std::string& text) {
        if (KeywordUtils::containsAny(text, Constants::SENTIMENT_KEYWORDS[u8"긍정"])) {
            return u8"긍정";
        }
        if (KeywordUtils::containsAny(text, Constants::SENTIMENT_KEYWORDS[u8"부정"])) {
            return u8"부정";
        }
        return u8"중립";
    }
};
