#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "Feedback.h"
#include "FeedbackClassifier.h"

class Filters {
public:
    std::vector<Feedback> filter(const std::vector<Feedback>& dataList,
                                 const std::string& sFilter,
                                 const std::string& kFilter) {
        std::vector<Feedback> tmpFiltered;

        if (sFilter != u8"전체") {
            for (const auto& item : dataList) {
                if (FeedbackClassifier::classifySentiment(item.getText()) == sFilter) {
                    tmpFiltered.push_back(item);
                }
            }
        } else {
            tmpFiltered = dataList;
        }

        std::vector<Feedback> finalFiltered;
        if (kFilter != u8"전체") {
            for (const auto& item : tmpFiltered) {
                if (FeedbackClassifier::matchCategory(item.getText(), kFilter)) {
                    finalFiltered.push_back(item);
                }
            }
        } else {
            finalFiltered = tmpFiltered;
        }

        for (const auto& i : finalFiltered) {
            std::cout << i.getText() << std::endl;
        }

        return finalFiltered;
    }
};
