#include <filesystem>
#include <iostream>
#include "utils/utils.h"
#include "utils/log/Logger.h"

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

std::string selectFileFromDir(const std::string& directoryPath, char preSelection) {
    if (!std::filesystem::is_directory(directoryPath)) {
        TET_CRITICAL("'{}' is not a directory", directoryPath);
    }

    std::vector<std::string> files{};
    for (const auto& file : std::filesystem::directory_iterator(directoryPath)) {
        const std::string path = file.path().generic_string();
        if (!file.is_regular_file()) {
            TET_WARN("Special file detected '{}'", path);
            continue;
        }

        auto lastSlash = path.find_last_of("/\\");
        files.emplace_back(path.substr((lastSlash == std::string::npos) ? 0 : lastSlash+1));
    }

    if (files.empty()) {
        TET_CRITICAL("No files to select!");
    }

    const std::string selectionChars = "0123456789qwertzuiopasdfghjklyxcvbnm";

    if (preSelection != '\0') {
        auto idx = selectionChars.find(preSelection);
        if (idx == std::string::npos || idx >= files.size()) {
            TET_WARN("Invalid selection");
        } else {
            return directoryPath + "/" + files[idx];
        }
    }

    for (int i = 0; i < files.size(); i++) {
        std::cout << "[" << selectionChars[i] << "] " << files[i] << std::endl;
    }

    std::string line;
    while (true) {
        std::getline(std::cin, line);
        if (std::cin.bad() || std::cin.fail()) {
            TET_CRITICAL("Input was interrupted");
            std::cin.clear();
            continue;
        }

        if (line.size() != 1) {
            TET_WARN("Expected exactly one character", line.size());
            continue;
        }

        auto idx = selectionChars.find(line);
        if (idx == std::string::npos || idx >= files.size()) {
            TET_WARN("Invalid selection");
            continue;
        }

        return directoryPath + "/" + files[idx];
    }
}
