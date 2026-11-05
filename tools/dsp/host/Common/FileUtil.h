#pragma once
#include <fstream>
#include <iterator>
#include <string>
namespace File {
inline bool ReadFileToString(const std::string& path, std::string& text) {
    std::ifstream input(path, std::ios::binary);
    if (!input) return false;
    text.assign(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
    return !input.bad();
}
}
