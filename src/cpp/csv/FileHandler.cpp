#include "FileHandler.h"

#include <fstream>

#include "CsvExporter.h"

bool FileHandler::saveToCsv(const std::vector<Feedback>& data, const std::string& path) {
    std::ofstream ofs(path, std::ios::binary);
    if (!ofs.is_open()) {
        return false;
    }
    ofs << CsvExporter::exportFilteredFeedbacks(data);
    return true;
}
