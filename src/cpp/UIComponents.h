#pragma once

#include <string>
#include <vector>

#include "KeywordRegistry.h"

class UIComponents {
public:
    static const std::vector<std::string>& getCategories() {
        return KeywordRegistry::categoryNames();
    }
};
