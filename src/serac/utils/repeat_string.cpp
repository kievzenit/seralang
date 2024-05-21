#include "repeat_string.h"

std::string utils::repeat_string(const std::string& str, int times) {
    if (times <= 0) {
        return "";
    }

    std::string result;
    result.reserve(str.size() * times);
    for (int i = 0; i < times; ++i) {
        result += str;
    }
    return result;
}