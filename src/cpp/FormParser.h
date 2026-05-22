#pragma once

#include <map>
#include <string>

class FormParser {
public:
    static std::map<std::string, std::string> parse(const std::string& body);

private:
    static std::string urlDecode(const std::string& str);
};
