#include "KeywordRegistry.h"

std::vector<std::string> KeywordRegistry::categoryNames_;

void KeywordRegistry::resetCategoryNames() {
    categoryNames_.clear();
}

void KeywordRegistry::registerCategoryName(const std::string& name) {
    categoryNames_.push_back(name);
}

const std::vector<std::string>& KeywordRegistry::categoryNames() {
    return categoryNames_;
}
