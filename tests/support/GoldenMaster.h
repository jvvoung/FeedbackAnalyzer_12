#pragma once

#include <cstdlib>
#include <fstream>
#include <iostream>
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
    int overflow(int ch) override { return ch == EOF ? 0 : ch; }
};

inline void suppressStdout() {
    static NullStreamBuf nullBuf;
    std::cout.rdbuf(&nullBuf);
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

inline std::string trimTrailingNewlines(std::string text) {
    while (!text.empty() && (text.back() == '\n' || text.back() == '\r')) {
        text.pop_back();
    }
    if (!text.empty() && text.back() != '\n') {
        text.push_back('\n');
    }
    return text;
}

inline std::string serializeSentimentMap(const std::map<std::string, int>& counts) {
    std::ostringstream oss;
    for (const auto& entry : counts) {
        oss << "sentiment|" << entry.first << "|" << entry.second << "\n";
    }
    return oss.str();
}

inline std::string serializeKeywordMap(const std::map<std::string, int>& counts) {
    std::ostringstream oss;
    for (const auto& entry : counts) {
        oss << "keyword|" << entry.first << "|" << entry.second << "\n";
    }
    return oss.str();
}

inline std::string serializeFilterResult(const std::vector<Feedback>& feedbacks) {
    std::ostringstream oss;
    oss << "count|" << feedbacks.size() << "\n";
    for (const auto& feedback : feedbacks) {
        oss << "text|" << feedback.getText() << "\n";
    }
    return oss.str();
}

inline std::string serializeAnalyzeResult(const std::map<std::string, int>& sentiment,
                                          const std::map<std::string, int>& keywords) {
    return serializeSentimentMap(sentiment) + serializeKeywordMap(keywords);
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

// Extract body for [GM-D-XX: ...] section (header line excluded, trailing --- excluded).
inline std::string loadGoldenSection(const std::string& fileContent, const std::string& sectionId) {
    const std::string marker = "[" + sectionId + ":";
    const std::size_t start = fileContent.find(marker);
    if (start == std::string::npos) {
        return {};
    }

    const std::size_t bodyStart = fileContent.find('\n', start);
    if (bodyStart == std::string::npos) {
        return {};
    }

    std::size_t end = fileContent.find("\n---", bodyStart + 1);
    if (end == std::string::npos) {
        end = fileContent.size();
    }

    std::string body = fileContent.substr(bodyStart + 1, end - bodyStart - 1);
    return trimTrailingNewlines(body);
}

inline std::string sectionHeader(const std::string& sectionId, const std::string& title) {
    return "[" + sectionId + ": " + title + "]\n";
}

inline std::string buildGoldenFile() {
    std::ostringstream out;
    out << sectionHeader("GM-D-01", "AnalyzeNegativeDelivery");
    {
        std::vector<Feedback> feedbacks = {Feedback(u8"배송이 너무 늦어요. 화가 납니다.")};
        TextAnalyzer analyzer;
        out << serializeAnalyzeResult(analyzer.sent(feedbacks), analyzer.kw(feedbacks));
    }
    out << "---\n";
    out << sectionHeader("GM-D-02", "FilterNeutral");
    {
        std::vector<Feedback> feedbacks = {Feedback(u8"그냥 그래요. 특별한 감정 없음.")};
        Filters filters;
        suppressStdout();
        out << serializeFilterResult(filters.fil(feedbacks, u8"중립", u8"전체"));
    }
    out << "---\n";
    out << sectionHeader("GM-D-03", "CsvParseTextColumn");
    {
        CsvParser parser;
        out << serializeFilterResult(parser.parse("text\n택배가 빨라요.\n").feedbacks);
    }
    out << "---\n";
    out << sectionHeader("GM-D-04", "FilterAll");
    {
        std::vector<Feedback> feedbacks = {
            Feedback(u8"좋아요 만족합니다."),
            Feedback(u8"배송이 너무 늦어요."),
            Feedback(u8"그냥 그래요. 특별한 감정 없음."),
        };
        Filters filters;
        suppressStdout();
        out << serializeFilterResult(filters.fil(feedbacks, u8"전체", u8"전체"));
    }
    out << "---\n";
    return out.str();
}

inline std::vector<std::string> splitLines(const std::string& text) {
    std::vector<std::string> lines;
    std::istringstream stream(text);
    std::string line;
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        lines.push_back(line);
    }
    return lines;
}

inline std::string printUnifiedDiff(const std::string& sectionId,
                                    const std::string& expected,
                                    const std::string& actual) {
    const auto expectedLines = splitLines(expected);
    const auto actualLines = splitLines(actual);

    std::ostringstream diff;
    diff << "--- expected (" << sectionId << ")\n";
    diff << "+++ actual\n";

    const std::size_t maxLines = std::max(expectedLines.size(), actualLines.size());
    std::size_t hunkStart = 0;
    bool inHunk = false;

    auto flushHunk = [&](std::size_t hunkEnd) {
        if (!inHunk) {
            return;
        }
        const std::size_t oldCount = hunkEnd - hunkStart;
        const std::size_t newCount = oldCount;
        diff << "@@ -" << (hunkStart + 1) << "," << oldCount << " +" << (hunkStart + 1) << ","
             << newCount << " @@\n";
        for (std::size_t i = hunkStart; i < hunkEnd; ++i) {
            const std::string expLine = i < expectedLines.size() ? expectedLines[i] : "";
            const std::string actLine = i < actualLines.size() ? actualLines[i] : "";
            if (expLine != actLine) {
                if (i < expectedLines.size()) {
                    diff << "-" << expectedLines[i] << "\n";
                }
                if (i < actualLines.size()) {
                    diff << "+" << actualLines[i] << "\n";
                }
            } else {
                diff << " " << expectedLines[i] << "\n";
            }
        }
        inHunk = false;
    };

    for (std::size_t i = 0; i < maxLines; ++i) {
        const std::string expLine = i < expectedLines.size() ? expectedLines[i] : "";
        const std::string actLine = i < actualLines.size() ? actualLines[i] : "";
        if (expLine != actLine) {
            if (!inHunk) {
                hunkStart = i;
                inHunk = true;
            }
        } else if (inHunk) {
            flushHunk(i);
        }
    }
    if (inHunk) {
        flushHunk(maxLines);
    }

    return diff.str();
}

inline bool compareGoldenSection(const std::string& sectionId,
                                 const std::string& expected,
                                 const std::string& actual,
                                 std::string* diffOut = nullptr) {
    const std::string normExpected = trimTrailingNewlines(expected);
    const std::string normActual = trimTrailingNewlines(actual);
    if (normExpected == normActual) {
        return true;
    }
    if (diffOut != nullptr) {
        *diffOut = printUnifiedDiff(sectionId, normExpected, normActual);
    }
    return false;
}

inline bool goldenUpdateEnabled() {
    const char* flag = std::getenv("GOLDEN_UPDATE");
    return flag != nullptr && std::string(flag) == "1";
}

inline bool updateGoldenFile(const std::string& path) {
    return writeFile(path, buildGoldenFile());
}

// Per-scenario capture helpers (body only, no section header).
inline std::string captureGmD01Body() {
    std::vector<Feedback> feedbacks = {Feedback(u8"배송이 너무 늦어요. 화가 납니다.")};
    TextAnalyzer analyzer;
    return serializeAnalyzeResult(analyzer.sent(feedbacks), analyzer.kw(feedbacks));
}

inline std::string captureGmD02Body() {
    std::vector<Feedback> feedbacks = {Feedback(u8"그냥 그래요. 특별한 감정 없음.")};
    Filters filters;
    suppressStdout();
    return serializeFilterResult(filters.fil(feedbacks, u8"중립", u8"전체"));
}

inline std::string captureGmD03Body() {
    CsvParser parser;
    return serializeFilterResult(parser.parse("text\n택배가 빨라요.\n").feedbacks);
}

inline std::string captureGmD04Body() {
    std::vector<Feedback> feedbacks = {
        Feedback(u8"좋아요 만족합니다."),
        Feedback(u8"배송이 너무 늦어요."),
        Feedback(u8"그냥 그래요. 특별한 감정 없음."),
    };
    Filters filters;
    suppressStdout();
    return serializeFilterResult(filters.fil(feedbacks, u8"전체", u8"전체"));
}

}  // namespace golden
