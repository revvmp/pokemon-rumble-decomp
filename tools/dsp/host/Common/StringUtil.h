#pragma once
#include <algorithm>
#include <cctype>
#include <string>
namespace Common {
inline void ToLower(std::string* text) {
    std::transform(text->begin(), text->end(), text->begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
}
}
