#pragma once
#include <string>
#include <vector>
#include <map>
#include "Feedback.h"
#include "FeedbackClassifier.h"
#include "KeywordRegistry.h"

class TextAnalyzer {
public:
    std::map<std::string, int> analyzeSentiment(const std::vector<Feedback>& feedbacks) {
        std::map<std::string, int> sentimentCounts;
        sentimentCounts[u8"긍정"] = 0;
        sentimentCounts[u8"중립"] = 0;
        sentimentCounts[u8"부정"] = 0;

        for (const auto& feedback : feedbacks) {
            sentimentCounts[FeedbackClassifier::classifySentiment(feedback.getText())]++;
        }

        return sentimentCounts;
    }

    std::map<std::string, int> analyzeKeywords(const std::vector<Feedback>& feedbacks) {
        std::map<std::string, int> keywordCounts;
        for (const auto& category : KeywordRegistry::categoryNames()) {
            keywordCounts[category] = 0;
        }

        for (const auto& feedback : feedbacks) {
            const std::string& text = feedback.getText();
            for (const auto& category : KeywordRegistry::categoryNames()) {
                if (FeedbackClassifier::matchCategory(text, category)) {
                    keywordCounts[category]++;
                }
            }
        }

        return keywordCounts;
    }
};
