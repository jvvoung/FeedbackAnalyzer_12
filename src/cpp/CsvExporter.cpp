#include "CsvExporter.h"

#include <sstream>

namespace {

constexpr const char* kUtf8Bom = "\xEF\xBB\xBF";
constexpr const char* kCsvHeader = "text\n";
constexpr const char* kAttachmentFilename = "filtered_feedback.csv";

}  // namespace

std::string CsvExporter::exportFilteredFeedbacks(const std::vector<Feedback>& feedbacks) {
    std::ostringstream csv;
    csv << kUtf8Bom;
    csv << kCsvHeader;
    for (const auto& feedback : feedbacks) {
        csv << feedback.getText() << "\n";
    }
    return csv.str();
}

std::string CsvExporter::contentType() {
    return "text/csv; charset=UTF-8";
}

std::string CsvExporter::contentDisposition() {
    return std::string("attachment; filename=\"") + kAttachmentFilename + "\"";
}
