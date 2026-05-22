#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Constants.h"
#include "Feedback.h"
#include "FileHandler.h"
#include "SentimentClassifier.h"

namespace {

struct TrendRow {
    std::string date;
    std::string text;
    std::string sentiment;
};

std::string trim(const std::string& value) {
    const auto start = value.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return "";
    }
    const auto end = value.find_last_not_of(" \t\r\n");
    return value.substr(start, end - start + 1);
}

std::vector<std::string> splitCsvLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string current;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        const char ch = line[i];
        if (ch == '"') {
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                current.push_back('"');
                ++i;
            } else {
                inQuotes = !inQuotes;
            }
            continue;
        }
        if (ch == ',' && !inQuotes) {
            fields.push_back(trim(current));
            current.clear();
            continue;
        }
        current.push_back(ch);
    }
    fields.push_back(trim(current));
    return fields;
}

bool readTrendCsv(const std::string& path, std::vector<TrendRow>* rows) {
    std::ifstream ifs(path);
    if (!ifs.is_open()) {
        return false;
    }

    std::string line;
    if (!std::getline(ifs, line)) {
        return false;
    }

    while (std::getline(ifs, line)) {
        if (trim(line).empty()) {
            continue;
        }
        const auto fields = splitCsvLine(line);
        if (fields.size() < 2) {
            continue;
        }
        TrendRow row;
        row.date = fields[0];
        row.text = fields[1];
        row.sentiment = fields.size() >= 3 ? fields[2] : "";
        rows->push_back(std::move(row));
    }
    return true;
}

bool writeEvaluatedCsv(const std::string& path, const std::vector<TrendRow>& rows) {
    std::ofstream ofs(path, std::ios::binary);
    if (!ofs.is_open()) {
        return false;
    }

    ofs << "date,text,sentiment\n";
    for (const auto& row : rows) {
        ofs << row.date << ',' << row.text << ',' << row.sentiment << '\n';
    }
    return true;
}

std::vector<TrendRow> evaluateSentiments(const std::vector<TrendRow>& rows) {
    std::vector<TrendRow> evaluated = rows;
    for (auto& row : evaluated) {
        row.sentiment = SentimentClassifier::classifyWeighted(row.text);
    }
    return evaluated;
}

}  // namespace

int main(int argc, char* argv[]) {
    Constants::init();

    const std::string repoRoot = (argc >= 2) ? argv[1] : ".";
    const std::string inputPath = repoRoot + "/data/test_feedback_trend.csv";
    const std::string evaluatedPath = repoRoot + "/data/feedback_evaluated.csv";
    const std::string exportPath = repoRoot + "/data/filtered_feedback.csv";

    std::vector<TrendRow> rows;
    if (!readTrendCsv(inputPath, &rows)) {
        std::cerr << "Failed to read trend CSV: " << inputPath << '\n';
        return 1;
    }

    const auto evaluated = evaluateSentiments(rows);
    if (!writeEvaluatedCsv(evaluatedPath, evaluated)) {
        std::cerr << "Failed to write evaluated CSV: " << evaluatedPath << '\n';
        return 1;
    }

    std::vector<Feedback> feedbacks;
    feedbacks.reserve(evaluated.size());
    for (const auto& row : evaluated) {
        feedbacks.emplace_back(row.text);
    }

    if (!FileHandler::saveToCsv(feedbacks, exportPath)) {
        std::cerr << "Failed to write FileHandler export: " << exportPath << '\n';
        return 1;
    }

    std::cout << "Generated:\n"
              << "  " << evaluatedPath << " (" << evaluated.size() << " rows)\n"
              << "  " << exportPath << " (FileHandler BOM+text export)\n";
    return 0;
}
