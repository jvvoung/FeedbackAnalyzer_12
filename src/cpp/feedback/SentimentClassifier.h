#pragma once

#include <string>

#include "Constants.h"

struct SentimentScore {
    int positive{0};
    int negative{0};
};

class SentimentClassifier {
public:
    static SentimentScore scoreWeighted(const std::string& text) {
        SentimentScore score;
        for (const auto& kw : Constants::SENTIMENT_KEYWORDS.at(u8"긍정")) {
            if (text.find(kw) != std::string::npos) {
                score.positive++;
            }
        }
        for (const auto& kw : Constants::SENTIMENT_KEYWORDS.at(u8"부정")) {
            if (text.find(kw) != std::string::npos) {
                score.negative++;
            }
        }
        return score;
    }

    static std::string classifyWeighted(const std::string& text) {
        const SentimentScore score = scoreWeighted(text);
        if (score.positive > score.negative) {
            return u8"긍정";
        }
        if (score.negative > score.positive) {
            return u8"부정";
        }
        return u8"중립";
    }

    static std::string classify(const std::string& text) {
        return classifyWeighted(text);
    }
};
