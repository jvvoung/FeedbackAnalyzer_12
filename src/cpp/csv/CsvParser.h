#pragma once

#include <string>
#include <vector>

#include "Feedback.h"

struct CsvParseResult {
    std::vector<Feedback> feedbacks;
    bool hasTextColumn{false};
    bool success{true};
    std::string error;
};

class CsvParser {
public:
    CsvParseResult parse(const std::string& csvContent);

private:
    static std::vector<std::string> parseLine(const std::string& line);
    static std::size_t findTextColumnIndex(const std::vector<std::string>& headerFields);
};
