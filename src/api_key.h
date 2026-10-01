#pragma once
#include <cstdlib>
#include <stdexcept>
#include <string>
inline std::string footballApiKey() {
    const char* value = std::getenv("FOOTBALL_DATA_API_KEY");
    if (!value || !*value) {
        throw std::runtime_error("Set FOOTBALL_DATA_API_KEY before running BallKnowledge.");
    }
    return std::string(value);
}
