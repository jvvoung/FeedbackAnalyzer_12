#pragma once
#include <string>
#include <vector>
#include <map>
#include "Feedback.h"
#include "Constants.h"
#include "FeedbackClassifier.h"
#include "KeywordRegistry.h"

class TextAnalyzer {
private:
    static std::map<std::string, int> globalSent;
    static std::map<std::string, int> globalKw;

public:
    std::map<std::string, int> sent(const std::vector<Feedback>& feedbacks) {
        std::map<std::string, int> res;
        res[u8"긍정"] = 0;
        res[u8"중립"] = 0;
        res[u8"부정"] = 0;

        for (const auto& f : feedbacks) {
            res[FeedbackClassifier::classifySentiment(f.getText())]++;
        }

        globalSent = res;
        return res;
    }

    std::map<std::string, int> kw(const std::vector<Feedback>& feedbacks) {
        std::map<std::string, int> res2;
        for (const auto& cat : KeywordRegistry::categoryNames()) {
            res2[cat] = 0;
        }

        for (const auto& f : feedbacks) {
            const std::string& txt = f.getText();
            for (const auto& cat : KeywordRegistry::categoryNames()) {
                if (FeedbackClassifier::matchCategory(txt, cat)) {
                    res2[cat]++;
                }
            }
        }

        globalKw = res2;
        return res2;
    }
};
