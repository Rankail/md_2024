#include <iostream>
#include <vector>
#include <filesystem>
#include <ranges>

#include <utils/log/Logger.h>
#include <utils/utils.h>
#include <utils/Arguments.h>
#include <utils/reader/InputReader.h>
#include <utils/reader/OutputReader.h>

std::string rightFill(std::string s, unsigned len) {
    if (s.length() >= len) return s;
    std::string app(len - s.length(), ' ');
    return s + app;
}

int main(int argc, char* argv[]) {
    Logger::init();

    if (!std::filesystem::is_directory(MD_INPUT_DIR)) {
        TET_CRITICAL("'{}' is not a directory", MD_INPUT_DIR);
    }

    std::vector<std::string> files{};
    unsigned maxNameLength = 0;
    for (const auto& file : std::filesystem::directory_iterator(MD_INPUT_DIR)) {
        const std::string path = file.path().generic_string();
        if (!file.is_regular_file()) {
            TET_WARN("Special file detected '{}'", path);
            continue;
        }

        auto lastSlash = path.find_last_of("/\\");
        const auto filename = path.substr((lastSlash == std::string::npos) ? 0 : lastSlash+1);
        maxNameLength = std::max((unsigned)filename.length(), maxNameLength);
        files.emplace_back(filename);
    }

    unsigned scoreSum = 0;
    for (const auto& filename : files) {
        auto inputData = InputReader::readFromFile(std::string(MD_INPUT_DIR) + '/' + filename);
        auto outputData = *OutputReader::readFromFile(std::string(MD_OUTPUT_DIR) + "/" + filename + ".out");

        auto edges = inputData->edges;

        const auto score = calculateScore2(inputData->nodes, outputData, edges);
        scoreSum += score;
        std::cout << rightFill(filename + ": ", maxNameLength + 2) << score << std::endl;
    }

    std::cout << std::string(maxNameLength + 10, '-') << std::endl;
    std::cout << rightFill("Sum: ", maxNameLength + 2) << scoreSum << std::endl;

    return 0;
}