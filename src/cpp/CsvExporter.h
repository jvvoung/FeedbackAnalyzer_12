#pragma once

#include <string>
#include <vector>

#include "Feedback.h"

class CsvExporter {
public:
    static std::string exportFilteredFeedbacks(const std::vector<Feedback>& feedbacks);
    static std::string contentType();
    static std::string contentDisposition();
};
