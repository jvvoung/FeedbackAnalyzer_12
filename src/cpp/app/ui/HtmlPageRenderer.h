#pragma once

#include <map>
#include <string>
#include <vector>

#include "Feedback.h"

struct PageViewModel {
    std::string success;
    std::string warning;
    std::string error;
    std::map<std::string, int> sentimentResults;
    std::map<std::string, int> keywordResults;
};

class HtmlPageRenderer {
public:
    static std::string render(const PageViewModel& viewModel);
};
