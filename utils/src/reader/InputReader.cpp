#include "utils/reader/InputReader.h"

#include <fstream>
#include <filesystem>
#include <numbers>

#include "utils/log/Logger.h"
#include "utils/utils.h"

FullInputData* InputReader::readFromFile(const std::string& path) {
    std::ifstream file{path, std::ios::in};

    if (!file.is_open()) {
        TET_ERROR("Unable to open file '{}' executed from '{}'", path, std::filesystem::current_path().generic_string());
        file.close();
        return nullptr;
    }

    auto data = new FullInputData();

    std::string line;
    unsigned lineNum = 0;
    std::unordered_map<std::string, unsigned> nameIdxMapping{};

    while (std::getline(file, line)) {
        if (line.empty()) break;
        const auto node = readNodeLine(line, lineNum);
        lineNum++;

        data->nodes.emplace_back(node);
        nameIdxMapping[node.name] = node.index;
    }

    while (std::getline(file, line)) {
        const auto edge = readEdgeLine(line, nameIdxMapping);
        data->edges.emplace_back(edge);
    }

    file.close();
    return data;
}

Node InputReader::readNodeLine(const std::string& line, unsigned lineNum) {
    const auto& data = splitAt(line, " \t");
    if (data.size() != 4) {
        TET_CRITICAL("Incorrect node-read-data at line {}. Got {} data-points instead of 4.", lineNum, data.size());
    }

    unsigned index{lineNum};
    std::string name{data[0]};
    double weight{std::stod(data[1])};
    weight = std::sqrt(weight/ std::numbers::pi);
    Vec2d originalPos{
        std::stod(data[2]),
        std::stod(data[3])
    };

    return {
        index, name, weight, originalPos
    };
}

uPair InputReader::readEdgeLine(const std::string& line,
    const std::unordered_map<std::string, unsigned int>& nameIdxMapping) {

    const auto& data = splitAt(line, " \t");
    if (data.size() != 2) {
        TET_CRITICAL("Incorrect edge-read-data. Got {} data-points instead of 2.", data.size());
    }

    auto name1 = data[0];
    auto name2 = data[1];
    auto it1 = nameIdxMapping.find(name1);
    auto it2 = nameIdxMapping.find(name2);
    auto pair = uPair{-1, -1};
    if (it1 == nameIdxMapping.end()) {
        TET_CRITICAL("Incorrect node-name '{}'.", name1);
    } else {
        pair.first = it1->second;
    }

    if (it2 == nameIdxMapping.end()) {
        TET_CRITICAL("Incorrect node-name '{}'.", name2);
    } else {
        pair.second = it2->second;
    }

    return pair;
}
