#pragma once

#include <string>
#include <vector>

class KeywordRegistry {
public:
    static void resetCategoryNames();
    static void registerCategoryName(const std::string& name);
    static const std::vector<std::string>& categoryNames();

private:
    static std::vector<std::string> categoryNames_;
};
