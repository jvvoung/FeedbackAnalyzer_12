#pragma once

#include <string>

namespace ServerConfig {

inline constexpr const char* kBindHost = "0.0.0.0";
inline constexpr int kPort = 8080;

inline std::string publicUrl() {
    return std::string("http://localhost:") + std::to_string(kPort);
}

}  // namespace ServerConfig
