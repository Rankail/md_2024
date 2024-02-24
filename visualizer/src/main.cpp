#include <filesystem>

#include <utils/log/Logger.h>
#include <utils/reader/InputReader.h>
#include <utils/utils.h>
#include "Visualizer.h"
#include "utils/reader/OutputReader.h"
#include "utils/Arguments.h"

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

int main(int argc, char** argv) {
    Logger::init();

    Arguments args{argc, argv};
    char filePreselection = '\0';
    if (args.getArgCount() == 1) {
        args >> filePreselection;
    }

    const std::string inputFilePath = selectFileFromDir(MD_INPUT_DIR, filePreselection);
    std::cout << inputFilePath << std::endl;

    auto lastSlash = inputFilePath.find_last_of("/\\");
    const auto& inputFilename = inputFilePath.substr(lastSlash+1);

    spdlog::stopwatch sw;

    auto inputData = InputReader::readFromFile(inputFilePath);
    inputData->nodes = *OutputReader::readFromFile(std::string(MD_OUTPUT_DIR) + "/" + inputFilename + ".out");

    TET_TRACE("Parsed input and output in {}s", sw);
    sw.reset();

    const auto graphicData = inputDataToGraphicData(*inputData, {1600, 900});

    TET_TRACE("Converted data to graphic in {}s", sw);
    sw.reset();

    auto visualizer = Visualizer();
    visualizer.init({1600, 900});
    visualizer.render(graphicData);

    return 0;
}
