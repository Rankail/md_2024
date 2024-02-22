#include "utils.h"

std::vector<std::string> splitAt(const std::string& s, const std::string& delims) {
    std::vector<std::string> parts{};

    auto startPos = s.find_first_not_of(delims);
    auto endPos = s.find_first_of(delims, startPos);
    while (endPos != std::string::npos) {
        parts.emplace_back(s.substr(startPos, endPos - startPos));
        startPos = s.find_first_not_of(delims, endPos);
        if (startPos == std::string::npos) {
            return parts;
        }
        endPos = s.find_first_of(delims, startPos);
    }

    parts.emplace_back(s.substr(startPos));

    return parts;
}
