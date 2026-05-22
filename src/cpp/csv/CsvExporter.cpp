#include "CsvExporter.h"

#include <sstream>

#include "CsvConstants.h"

namespace {

std::string escapeCsvField(const std::string& field) {
    const bool needsQuotes = field.find_first_of(",\"\n\r") != std::string::npos;
    if (!needsQuotes) {
        return field;
    }

    std::string escaped;
    escaped.reserve(field.size() + 2);
    escaped.push_back('"');
    for (char character : field) {
        if (character == '"') {
            escaped.append("\"\"");
        } else {
            escaped.push_back(character);
        }
    }
    escaped.push_back('"');
    return escaped;
}

std::string buildCsvBody(const std::vector<Feedback>& feedbacks) {
    std::ostringstream csv;
    csv << CsvConstants::kUtf8Bom;
    csv << CsvConstants::kTextColumnHeader;
    for (const auto& feedback : feedbacks) {
        csv << escapeCsvField(feedback.getText()) << "\n";
    }
    return csv.str();
}

}  // namespace

std::string CsvExporter::exportFilteredFeedbacks(const std::vector<Feedback>& feedbacks) {
    return buildCsvBody(feedbacks);
}

std::string CsvExporter::contentType() {
    return CsvConstants::kContentType;
}

std::string CsvExporter::contentDisposition() {
    return std::string("attachment; filename=\"") + CsvConstants::kAttachmentFilename + "\"";
}
