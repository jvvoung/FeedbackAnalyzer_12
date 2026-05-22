#include "CsvParser.h"

#include <sstream>

namespace {

std::vector<std::string> splitCsvRecords(const std::string& csvContent) {
    std::vector<std::string> records;
    std::string record;
    bool inQuotes = false;

    for (std::size_t i = 0; i < csvContent.size(); ++i) {
        const char ch = csvContent[i];
        if (ch == '"') {
            if (inQuotes && i + 1 < csvContent.size() && csvContent[i + 1] == '"') {
                record.push_back('"');
                ++i;
            } else {
                inQuotes = !inQuotes;
                record.push_back(ch);
            }
        } else if ((ch == '\n' || ch == '\r') && !inQuotes) {
            if (ch == '\r' && i + 1 < csvContent.size() && csvContent[i + 1] == '\n') {
                ++i;
            }
            if (!record.empty()) {
                records.push_back(record);
                record.clear();
            }
        } else {
            record.push_back(ch);
        }
    }

    if (!record.empty()) {
        records.push_back(record);
    }
    return records;
}

std::string unquoteField(std::string field) {
    if (field.size() >= 2 && field.front() == '"' && field.back() == '"') {
        field = field.substr(1, field.size() - 2);
        std::string unescaped;
        unescaped.reserve(field.size());
        for (std::size_t i = 0; i < field.size(); ++i) {
            if (field[i] == '"' && i + 1 < field.size() && field[i + 1] == '"') {
                unescaped.push_back('"');
                ++i;
            } else {
                unescaped.push_back(field[i]);
            }
        }
        return unescaped;
    }
    return field;
}

}  // namespace

std::vector<std::string> CsvParser::parseLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    bool inQuotes = false;

    for (std::size_t i = 0; i < line.size(); ++i) {
        const char ch = line[i];
        if (ch == '"') {
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                field.push_back('"');
                ++i;
            } else {
                inQuotes = !inQuotes;
                field.push_back(ch);
            }
        } else if (ch == ',' && !inQuotes) {
            fields.push_back(field);
            field.clear();
        } else {
            field.push_back(ch);
        }
    }
    fields.push_back(field);
    return fields;
}

std::size_t CsvParser::findTextColumnIndex(const std::vector<std::string>& headerFields) {
    for (std::size_t i = 0; i < headerFields.size(); ++i) {
        if (headerFields[i] == "text") {
            return i;
        }
    }
    return static_cast<std::size_t>(-1);
}

CsvParseResult CsvParser::parse(const std::string& csvContent) {
    CsvParseResult result;
    const auto records = splitCsvRecords(csvContent);
    if (records.empty()) {
        return result;
    }

    const auto headerFields = parseLine(records[0]);
    std::size_t textIndex = findTextColumnIndex(headerFields);

    // RED(H-2): text 컬럼 없으면 첫 번째 컬럼 사용 — AC-2 위반, T-05 failing
    const bool useFallbackFirstColumn = (textIndex == static_cast<std::size_t>(-1));
    if (useFallbackFirstColumn) {
        textIndex = 0;
    }

    for (std::size_t row = 1; row < records.size(); ++row) {
        const auto fields = parseLine(records[row]);
        if (fields.size() <= textIndex) {
            continue;
        }
        std::string textValue = unquoteField(fields[textIndex]);
        if (!textValue.empty()) {
            result.feedbacks.emplace_back(textValue);
        }
    }

    return result;
}
