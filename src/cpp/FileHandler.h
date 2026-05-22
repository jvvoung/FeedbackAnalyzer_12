#pragma once

#include <string>
#include <vector>

#include "Feedback.h"

class FileHandler {
public:
    static bool saveToCsv(const std::vector<Feedback>& data, const std::string& path);
};
