#include <SDL.h>
#include <filesystem>

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
        minX = std::min(minX, circle->originalPosition.x() - circle->radius);
        minY = std::min(minY, circle->originalPosition.y() - circle->radius);
        maxX = std::max(maxX, circle->originalPosition.x() + circle->radius);
        maxY = std::max(maxY, circle->originalPosition.y() + circle->radius);
    }
    Vec2d positionOffset = Vec2d(minX, maxY);
    double scaleFactor = std::min(1 / (maxX - minX) * targetSize.x(), 1 / (maxY - minY) * targetSize.y());

    auto graphicData = FullGraphicData();
    for (const auto& circle : inputData.nodes) {
        auto newCircle = *circle;
        newCircle.originalPosition -= positionOffset;
        newCircle.originalPosition.x() *= scaleFactor;
        newCircle.originalPosition.y() *= -scaleFactor;
        newCircle.radius *= scaleFactor;
        graphicData.circles.emplace_back(newCircle);
    }

    for (const auto& [idx1, idx2] : inputData.edges) {
        bool touching = false;
        const auto& node1 = graphicData.circles[idx1];
        const auto& node2 = graphicData.circles[idx2];
        const auto diff = (node1.originalPosition - node2.originalPosition).abs();
        const auto a2b2 = diff.x() * diff.x() + diff.y() * diff.y();
        const auto c2 = std::pow(node1.radius + node2.radius, 2.);
        if (a2b2 <= c2) {
            touching = true;
        }
        auto edge = GraphicEdge();
        edge.touching = touching;
        edge.line = std::make_pair(node1.originalPosition, node2.originalPosition);
        graphicData.lines.emplace_back(edge);
    }
    return graphicData;
}

std::string selectFileFromDir(const std::string& directoryPath) {
    if (!std::filesystem::is_directory(directoryPath)) {
        TET_CRITICAL("'{}' is not a directory", directoryPath);
    }

    std::vector<std::string> files{};
    for (const auto& file : std::filesystem::directory_iterator(directoryPath)) {
        if (!file.is_regular_file()) {
            TET_WARN("Special file detected '{}'", file.path().generic_string());
            continue;
        }
        files.emplace_back(file.path().generic_string().substr(directoryPath.size()+1));
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

int main(int argc, char** argv) {
    Logger::init();

    const std::string inputFilePath = selectFileFromDir("input_files");
    std::cout << inputFilePath << std::endl;

    auto inputData = InputReader::readFromFile(inputFilePath);

    //OutputPrinter::printToFile("output_files/Area_Afro-Eurasia.txt", inputData->nodes);

    const auto graphicData = inputDataToGraphicData(*inputData);

    auto visualizer = Visualizer();
    visualizer.init();
    visualizer.render(graphicData);

    return 0;
}
