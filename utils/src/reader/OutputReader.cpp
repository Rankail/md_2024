#include "utils/reader/OutputReader.h"

#include <unordered_map>
#include <filesystem>
#include <fstream>

#include "utils/log/Logger.h"
#include "utils/utils.h"

std::vector<Node> *OutputReader::readFromFile(const std::string& path) {
    std::ifstream file{path, std::ios::in};

    if (!file.is_open()) {
        TET_ERROR("Unable to open file '{}' executed from '{}'", path, std::filesystem::current_path().generic_string());
        file.close();
        return nullptr;
    }

    auto data = new std::vector<Node>();

    std::string line;
    unsigned lineNum = 0;
    while (std::getline(file, line)) {
        const auto node = readNodeLine(line, lineNum);
        lineNum++;

        data->emplace_back(node);
    }

    file.close();
    return data;
}

Node OutputReader::readNodeLine(const std::string& line, unsigned int lineNum) {
    const auto& data = splitAt(line, " \t");
    if (data.size() != 5) {
        TET_CRITICAL("Incorrect node-read-data at line {}. Got {} data-points instead of 5.", lineNum, data.size());
    }

    Vec2d position{
        std::stod(data[0]),
        std::stod(data[1])
    };
    double radius{std::stod(data[2])};
    std::string name{data[3]};
    unsigned index{std::stoul(data[4])};

    return {
        index, name, radius, position
    };
}
