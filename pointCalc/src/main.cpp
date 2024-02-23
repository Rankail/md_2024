#include <iostream>
#include <vector>
#include <filesystem>
#include <ranges>

#include <utils/log/Logger.h>
#include <utils/utils.h>
#include <utils/reader/InputReader.h>
#include <utils/reader/OutputReader.h>

int main(int argc, char* argv[]) {
    Logger::init();

    const std::string inputFilePath = selectFileFromDir(MD_INPUT_DIR);
    std::cout << inputFilePath << std::endl;

    auto lastSlash = inputFilePath.find_last_of("/\\");
    const auto& filename = inputFilePath.substr(lastSlash+1);

    spdlog::stopwatch sw;

    auto inputData = InputReader::readFromFile(inputFilePath);
    auto outputData = *OutputReader::readFromFile(std::string(MD_OUTPUT_DIR) + "/" + filename + ".out");

    TET_TRACE("Parsed input and output in {}s", sw);
    sw.reset();

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

    TET_TRACE("Calculated score in {}s", sw.elapsed().count());

    std::cout   << "Max overlap: " << maxOverlap << std::endl
                << "Max distance: " << maxDistance << std::endl
                << "Max angle: " << maxAngle << std::endl
                << "Score: " << scoreRounded << std::endl;

    return 0;
}