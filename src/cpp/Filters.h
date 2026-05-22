#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "Feedback.h"
#include "Constants.h"
#include "KeywordUtils.h"
#include "SentimentClassifier.h"

class Filters {
public:
    std::vector<Feedback> fil(const std::vector<Feedback>& dataList,
                              const std::string& sFilter,
                              const std::string& kFilter) {
        std::vector<Feedback> tmpFiltered;

        if (sFilter != u8"전체") {
            for (const auto& item : dataList) {
                if (SentimentClassifier::classify(item.getText()) == sFilter) {
                    tmpFiltered.push_back(item);
                }
            }
        } else {
            tmpFiltered = dataList;
        }

        std::vector<Feedback> finalFiltered;
        if (kFilter != u8"전체") {
            for (const auto& item : tmpFiltered) {
                std::string txt = item.getText();
                if (Constants::CATEGORY_KEYWORDS.count(kFilter)) {
                    const auto& catMap = Constants::CATEGORY_KEYWORDS.at(kFilter);
                    if (catMap.count("main") && KeywordUtils::containsAny(txt, catMap.at("main"))) {
                        finalFiltered.push_back(item);
                    }
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
