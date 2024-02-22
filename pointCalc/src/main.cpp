#include <iostream>
#include <vector>
#include <filesystem>
#include <unordered_set>
#include <ranges>
#include <spdlog/stopwatch.h>

#include "InputReader.h"
#include "OutputReader.h"
#include "Logger.h"
#include "utils.h"

std::string selectFileFromDir(const std::string& directoryPath) {
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

int main(int argc, char* argv[]) {
    Logger::init();

    const std::string inputFilePath = selectFileFromDir(MD_INPUT_DIR);
    std::cout << inputFilePath << std::endl;

    auto lastSlash = inputFilePath.find_last_of("/\\");
    const auto& filename = inputFilePath.substr(lastSlash+1);

    spdlog::stopwatch sw;

    auto inputData = InputReader::readFromFile(inputFilePath);
    TET_TRACE("Finished parsing input at {}", sw);
    auto outputData = *OutputReader::readFromFile(MD_OUTPUT_DIR + filename + ".out");
    TET_TRACE("Finished parsing output at {}", sw);

    auto edges = inputData->edges;
    auto edgesFilteredView = edges | std::ranges::views::filter([](const uPair& p) {
        return p.first < p.second;
    });

    auto edgesFiltered = std::vector<uPair>{edgesFilteredView.begin(), edgesFilteredView.end()};
    std::ranges::sort(edgesFiltered, [](const uPair& a, const uPair& b) {
        if (a.first < b.first) return true;
        if (a.first == b.first) return a.second < b.second;
        return false;
    });

    unsigned edgeIdx = 0;
    double maxDistance = 0.0;
    double maxOverlap = 0.0;
    double maxAngle = 0.0;

    for (int i = 0; i < outputData.size()-1; i++) {
        for (int j = i + 1; j < outputData.size(); j++) {
            const auto n1 = outputData[i];
            const auto n2 = outputData[j];
            const auto diff = n1->position - n2->position;
            const auto a2b2 = diff.x() * diff.x() + diff.y() * diff.y();
            const auto r12 = n1->radius + n2->radius;
            const auto c2 = r12 * r12;
            if (a2b2 < c2) {
                const auto dist = std::sqrt(a2b2);
                const auto overlap = (r12 - dist) / r12;
                maxOverlap = std::max(maxOverlap, overlap);
            }
            if (edgeIdx < edgesFiltered.size() && edgesFiltered[edgeIdx].first == i && edgesFiltered[edgeIdx].second == j) {
                if (a2b2 > c2) {
                    const auto dist = std::sqrt(a2b2);
                    const auto unwantedDist = (dist - r12) / r12;
                    maxDistance = std::max(maxDistance, unwantedDist);
                }

                const auto in1 = inputData->nodes[i];
                const auto in2 = inputData->nodes[j];
                const auto inDiff = in1->position - in2->position;
                const auto angle = radToDeg(inDiff.smallestAngleTo(diff));
                maxAngle = std::max(maxAngle, (double)angle);
                edgeIdx++;
            }
        }
    }

    const auto overlapFactor = maxOverlap * 200;
    const auto distanceFactor = maxDistance * 100;
    const auto angleFactor = maxAngle / 18.0;

    const auto score = 1000.0 * (outputData.size() + edges.size()) / (1 + overlapFactor + distanceFactor + angleFactor);
    const auto scoreRounded = (int)(score + 0.5);

    TET_TRACE("Finished score calculation at {}", sw.elapsed().count());

    std::cout   << "Max overlap: " << maxOverlap << std::endl
                << "Max distance: " << maxDistance << std::endl
                << "Max angle: " << maxAngle << std::endl
                << "Score: " << scoreRounded << std::endl;

    return 0;
}