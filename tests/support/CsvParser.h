#pragma once

#include <string>
#include <vector>

#include "Feedback.h"

// RED 단계 테스트용 CsvParser stub (src/cpp/main.cpp 미수정, 독립 구현)
// H-2 재현: text 컬럼 없을 때 fields[0] fallback — T-05 failing 유지용
struct CsvParseResult {
    std::vector<Feedback> feedbacks;
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
