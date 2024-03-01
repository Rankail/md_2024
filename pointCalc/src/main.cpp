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
        auto edgesFilteredView = edges | std::ranges::views::filter(
            [](const uPair& p) {
                return p.first < p.second;
            }
        );

        auto edgesFiltered = std::vector<uPair>{edgesFilteredView.begin(), edgesFilteredView.end()};
        std::ranges::sort(
            edgesFiltered, [](const uPair& a, const uPair& b) {
                if (a.first < b.first) return true;
                if (a.first == b.first) return a.second < b.second;
                return false;
            }
        );

        unsigned edgeIdx = 0;
        double maxDistance = 0.0;
        double maxOverlap = 0.0;
        double maxAngle = 0.0;

        for (int i = 0; i < outputData.size() - 1; i++) {
            for (int j = i + 1; j < outputData.size(); j++) {
                const auto& n1 = outputData[i];
                const auto& n2 = outputData[j];
                const auto diff = n1.position - n2.position;
                const auto a2b2 = diff.x() * diff.x() + diff.y() * diff.y();
                const auto r12 = n1.radius + n2.radius;
                const auto c2 = r12 * r12;
                if (a2b2 < c2) {
                    const auto dist = std::sqrt(a2b2);
                    const auto overlap = (r12 - dist) / r12;
                    maxOverlap = std::max(maxOverlap, overlap);
                }
                if (edgeIdx < edgesFiltered.size() && edgesFiltered[edgeIdx].first == i &&
                    edgesFiltered[edgeIdx].second == j) {
                    if (a2b2 > c2) {
                        const auto dist = std::sqrt(a2b2);
                        const auto unwantedDist = (dist - r12) / r12;
                        maxDistance = std::max(maxDistance, unwantedDist);
                    }

                    const auto& in1 = inputData->nodes[i];
                    const auto& in2 = inputData->nodes[j];
                    const auto inDiff = in1.position - in2.position;
                    const auto angle = radToDeg(inDiff.smallestAngleTo(diff));
                    maxAngle = std::max(maxAngle, (double) angle);
                    edgeIdx++;
                }
            }
        }

        const auto overlapFactor = maxOverlap * 200;
        const auto distanceFactor = maxDistance * 100;
        const auto angleFactor = maxAngle / 18.0;

        const auto score =
            1000.0 * (outputData.size() + edges.size()) / (1 + overlapFactor + distanceFactor + angleFactor);
        const auto scoreRounded = (int) (score + 0.5);

        scoreSum += scoreRounded;

        std::cout << rightFill(filename + ": ", maxNameLength + 2) << scoreRounded << std::endl;
    }

    std::cout << std::string(maxNameLength + 10, '-') << std::endl;
    std::cout << rightFill("Sum: ", maxNameLength + 2) << scoreSum << std::endl;

    return 0;
}