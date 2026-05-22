#pragma once

#include <fstream>
#include <map>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>

#include "Constants.h"
#include "CsvParser.h"
#include "Feedback.h"
#include "Filters.h"
#include "TextAnalyzer.h"

namespace golden {

class NullStreamBuf : public std::streambuf {
protected:
    int overflow(int ch) override { return ch; }
};

inline void suppressStdout() {
    static NullStreamBuf nullBuf;
    std::cout.rdbuf(&nullBuf);
}

inline void appendSentimentMap(std::ostringstream& oss, const std::map<std::string, int>& counts) {
    for (const auto& entry : counts) {
        oss << "sentiment|" << entry.first << "|" << entry.second << "\n";
    }
}

inline void appendKeywordMap(std::ostringstream& oss, const std::map<std::string, int>& counts) {
    for (const auto& entry : counts) {
        oss << "keyword|" << entry.first << "|" << entry.second << "\n";
    }
}

inline void appendFeedbackList(std::ostringstream& oss, const std::vector<Feedback>& feedbacks) {
    oss << "count|" << feedbacks.size() << "\n";
    for (const auto& feedback : feedbacks) {
        oss << "text|" << feedback.getText() << "\n";
    }
}

inline std::string scenarioBlock(const std::string& scenarioId, const std::string& body) {
    return "=== " + scenarioId + " ===\n" + body;
}

inline std::string captureGmD01() {
    std::vector<Feedback> feedbacks = {
        Feedback(u8"배송이 너무 늦어요. 화가 납니다."),
    };
    TextAnalyzer analyzer;
    std::ostringstream body;
    appendSentimentMap(body, analyzer.sent(feedbacks));
    appendKeywordMap(body, analyzer.kw(feedbacks));
    return scenarioBlock("GM-D-01", body.str());
}

inline std::string captureGmD02() {
    std::vector<Feedback> feedbacks = {
        Feedback(u8"그냥 그래요. 특별한 감정 없음."),
    };
    Filters filters;
    suppressStdout();
    std::ostringstream body;
    appendFeedbackList(body, filters.fil(feedbacks, u8"중립", u8"전체"));
    return scenarioBlock("GM-D-02", body.str());
}

inline std::string captureGmD03() {
    CsvParser parser;
    std::ostringstream body;
    appendFeedbackList(body, parser.parse("text\n택배가 빨라요.\n").feedbacks);
    return scenarioBlock("GM-D-03", body.str());
}

inline std::string captureGmD04() {
    std::vector<Feedback> feedbacks = {
        Feedback(u8"좋아요 만족합니다."),
        Feedback(u8"배송이 너무 늦어요."),
        Feedback(u8"그냥 그래요. 특별한 감정 없음."),
    };
    Filters filters;
    suppressStdout();
    std::ostringstream body;
    appendFeedbackList(body, filters.fil(feedbacks, u8"전체", u8"전체"));
    return scenarioBlock("GM-D-04", body.str());
}

inline std::string captureAllDomainGolden() {
    Constants::init();
    std::ostringstream out;
    out << captureGmD01();
    out << captureGmD02();
    out << captureGmD03();
    out << captureGmD04();
    return out.str();
}

inline std::string normalizeLineEndings(std::string text) {
    std::string normalized;
    normalized.reserve(text.size());
    for (std::size_t i = 0; i < text.size(); ++i) {
        if (text[i] == '\r') {
            if (i + 1 < text.size() && text[i + 1] == '\n') {
                ++i;
            }
            normalized.push_back('\n');
        } else {
            normalized.push_back(text[i]);
        }
    }
    return normalized;
}

inline std::string readFile(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        return {};
    }
    std::ostringstream contents;
    contents << in.rdbuf();
    return normalizeLineEndings(contents.str());
}

inline bool writeFile(const std::string& path, const std::string& contents) {
    std::ofstream out(path, std::ios::binary);
    if (!out) {
        return false;
    }
    out << normalizeLineEndings(contents);
    return static_cast<bool>(out);
}

}  // namespace golden
