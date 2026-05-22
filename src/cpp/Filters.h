#pragma once
#include <string>
#include <vector>
#include "Feedback.h"
#include "FeedbackClassifier.h"
#include "FilterConstants.h"

class Filters {
public:
    std::vector<Feedback> filter(const std::vector<Feedback>& dataList,
                                 const std::string& sentimentFilter,
                                 const std::string& keywordFilter) {
        std::vector<Feedback> sentimentFiltered;

        if (sentimentFilter != FilterConstants::kAllSentinel) {
            for (const auto& item : dataList) {
                if (FeedbackClassifier::classifySentiment(item.getText()) == sentimentFilter) {
                    sentimentFiltered.push_back(item);
                }
            }
        } else {
            sentimentFiltered = dataList;
        }

        std::vector<Feedback> keywordFiltered;
        if (keywordFilter != FilterConstants::kAllSentinel) {
            for (const auto& item : sentimentFiltered) {
                if (FeedbackClassifier::matchCategory(item.getText(), keywordFilter)) {
                    keywordFiltered.push_back(item);
                }
            }
        } else {
            keywordFiltered = sentimentFiltered;
        }

        return keywordFiltered;
    }
};
