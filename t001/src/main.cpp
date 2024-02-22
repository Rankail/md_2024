#include <SDL.h>
#include <filesystem>
#include <spdlog/stopwatch.h>

#include "Logger.h"
#include "input/InputReader.h"
#include "output/OutputPrinter.h"
#include "atlas/Visualizer.h"

FullGraphicData inputDataToGraphicData(const FullInputData& inputData, const Vec2d& targetSize = {800, 600}) {
    auto minX = std::numeric_limits<double>::max();
    auto minY = std::numeric_limits<double>::max();
    auto maxX = std::numeric_limits<double>::min();
    auto maxY = std::numeric_limits<double>::min();

    for (const auto& circle : inputData.nodes) {
        minX = std::min(minX, circle->position.x() - circle->radius);
        minY = std::min(minY, circle->position.y() - circle->radius);
        maxX = std::max(maxX, circle->position.x() + circle->radius);
        maxY = std::max(maxY, circle->position.y() + circle->radius);
    }
    Vec2d positionOffset = Vec2d(minX, maxY);
    double scaleFactor = std::min(1 / (maxX - minX) * targetSize.x(), 1 / (maxY - minY) * targetSize.y());

    auto graphicData = FullGraphicData();
    for (const auto& circle : inputData.nodes) {
        auto newCircle = *circle;
        newCircle.position -= positionOffset;
        newCircle.position.x() *= scaleFactor;
        newCircle.position.y() *= -scaleFactor;
        newCircle.radius *= scaleFactor;
        graphicData.circles.emplace_back(newCircle);
    }

    for (const auto& [idx1, idx2] : inputData.edges) {
        bool touching = false;
        const auto& node1 = graphicData.circles[idx1];
        const auto& node2 = graphicData.circles[idx2];
        const auto diff = (node1.position - node2.position).abs();
        const auto a2b2 = diff.x() * diff.x() + diff.y() * diff.y();
        const auto c2 = std::pow(node1.radius + node2.radius, 2.);
        if (a2b2 <= c2) {
            touching = true;
        }
        auto edge = GraphicEdge();
        edge.touching = touching;
        edge.line = std::make_pair(node1.position, node2.position);
        graphicData.lines.emplace_back(edge);
    }
    return graphicData;
}

std::string selectFileFromDir(const std::string& directoryPath, char preSelection = '\0') {
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

int main(int argc, char** argv) {
    Logger::init();

    auto preSelected = '\0';
    if (argc == 2) {
        const auto arg1 = std::string(argv[1]);
        if (arg1.size() == 1)
            preSelected = arg1[0];
    }

    const std::string inputFilePath = selectFileFromDir(MD_INPUT_DIR, preSelected);
    std::cout << inputFilePath << std::endl;

    auto lastSlash = inputFilePath.find_last_of("/\\");
    const auto& inputFilename = inputFilePath.substr(lastSlash+1);

    spdlog::stopwatch sw;
    auto inputData = InputReader::readFromFile(inputFilePath);

    TET_TRACE("Parsed input in {}", sw);
    sw.reset();

    OutputPrinter::printToFile(MD_OUTPUT_DIR + inputFilename + ".out", inputData->nodes);

    TET_TRACE("Wrote output in {}", sw);
    sw.reset();

    const auto graphicData = inputDataToGraphicData(*inputData);

    TET_TRACE("Converted input to graphic in {}", sw);
    sw.reset();

    auto visualizer = Visualizer();
    visualizer.init();
    visualizer.render(graphicData);

    return 0;
}
