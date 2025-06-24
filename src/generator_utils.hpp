#pragma once
#include <cctype>
#include <algorithm>
auto tolower(std::string s) -> std::string {
    std::transform(begin(s), end(s), begin(s), [](unsigned char c) { return std::tolower(c); });
    return s;
}
