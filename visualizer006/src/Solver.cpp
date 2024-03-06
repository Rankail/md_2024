#include "Solver.h"
#include "utils/log/Logger.h"

#include <algorithm>
#include <random>

#include <utils/reader/InputReader.h>
#include <utils/reader/OutputReader.h>
#include <utils/printer/OutputPrinter.h>

bool Solver::init(const std::string& filename) {
    this->filename = filename;
    auto inputData = InputReader::readFromFile(std::string(MD_INPUT_DIR) + '/' + filename);
    auto outputData = OutputReader::readFromFile(std::string(MD_OUTPUT_DIR) + '/' + filename + ".out");

    maxScore = calculateScore2(inputData->nodes, *outputData, inputData->edges);

    totalSteps = 0;

    original.clear();
    nodes.clear();
    edges.clear();

    overlapFactor = 0.1;
    distanceFactor = 0.001;
    angleFactor = 0.001;

    edgeInputCount = inputData->edges.size();

    for (const Node& node : inputData->nodes) {
        original.push_back(node);
        nodes.push_back(node);
    }

    edges = std::vector<std::vector<bool>>(nodes.size(), std::vector<bool>(nodes.size(), false));

    for (const auto [idx1, idx2] : inputData->edges) {
        edges[idx1][idx2] = true;
        edges[idx2][idx1] = true;
    }

    calculateWorstAndMakeGraphic();

    return true;
}

void Solver::calculateWorstAndMakeGraphic() {
    auto minX = std::numeric_limits<double>::max();
    auto minY = std::numeric_limits<double>::max();
    auto maxX = std::numeric_limits<double>::min();
    auto maxY = std::numeric_limits<double>::min();

    for (const auto& circle : nodes) {
        minX = std::min(minX, circle.position.x() - circle.radius);
        minY = std::min(minY, circle.position.y() - circle.radius);
        maxX = std::max(maxX, circle.position.x() + circle.radius);
        maxY = std::max(maxY, circle.position.y() + circle.radius);
    }
    Vec2d positionOffset = Vec2d(minX, minY);
    double scaleFactor = std::min(targetSize.x() / (maxX - minX), targetSize.y() / (maxY - minY));

    graphicData = FullGraphicData2();
    for (const auto& circle : nodes) {
        auto newCircle = circle;
        newCircle.position -= positionOffset;
        newCircle.position.x() *= scaleFactor * zoom;
        newCircle.position.y() *= scaleFactor * zoom;
        newCircle.radius *= scaleFactor * zoom;
        newCircle.position -= offset;
        graphicData.circles.emplace_back(newCircle);
    }

    double distanceSum = 0.0;
    double angleSum = 0.0;
    unsigned edgeCount = 0;

    for (int i = 0; i < nodes.size() - 1; i++) {
        for (int j = i + 1; j < nodes.size(); j++) {
            edgeCount++;
            const auto& node1 = graphicData.circles[i];
            const auto& node2 = graphicData.circles[j];
            const auto diff = node1.position - node2.position;
            const auto dist = std::hypot(diff.x(), diff.y());
            const auto r12 = node1.radius + node2.radius;
            if (dist < r12) {
                const auto overlap = (r12 - dist) / r12;
                if (overlap > graphicData.overlap) {
                    graphicData.worstOverlap = {i, j};
                    graphicData.overlap = overlap;
                }
            }
            if (!edges[i][j]) continue;
            if (dist > r12) {
                const auto distance = (dist - r12) / r12;
                distanceSum += distance;
            }

            const auto in1 = original[i];
            const auto in2 = original[j];
            const auto inDiff = in1.position - in2.position;
            const auto angle = inDiff.smallestAngleTo(diff) / std::numbers::pi;
            angleSum += angle;

            auto edge = GraphicEdge();
            edge.idxs = {i, j};
            graphicData.edges.emplace_back(edge);
        }
    }

    graphicData.angle = radToDeg(graphicData.angle);
    graphicData.overlap *= 100.0;
    graphicData.distance *= 100.0;

    graphicData.score = (1000 * (nodes.size() + edgeInputCount))
                        / (1 + 2 * graphicData.overlap + graphicData.distance + graphicData.angle / 18);

    if (maxScore < graphicData.score) {
        maxScore = graphicData.score;
        printToFile();
    }
    graphicData.maxScore = maxScore;

    graphicData.totalSteps = totalSteps;

    graphicData.distanceFactor = distanceFactor;
    graphicData.overlapFactor = overlapFactor;
    graphicData.angleFactor = angleFactor;
}

void Solver::run(unsigned iterations) {
    for (int i = 0; i < iterations; i++) {
        iteration();
        calculateWorstAndMakeGraphic();
    }
    bestRotation();
    totalSteps += iterations;
}

void Solver::findSmallestNotColliding() {
    std::vector<double> overlaps{};

    for (int i = 0; i < nodes.size()-1; i++) {
        const auto& n1 = nodes[i];
        for (int j = i + 1; j < nodes.size(); j++) {
            const auto& n2 = nodes[j];
            const auto diff = n1.position - n2.position;
            const auto dist = std::hypot(diff.x(), diff.y());
            const auto r12 = n1.radius + n2.radius;
            const auto overlap = (r12 - dist) / dist;
            overlaps.emplace_back(overlap);
        }
    }

    std::sort(overlaps.begin(), overlaps.end());
    const auto smallOverlap = overlaps[overlaps.size() * 99 / 100];

    const auto factor = 1 + smallOverlap;
    for (int i = 0; i < nodes.size(); i++) {
        nodes[i].position *= factor;
    }

    calculateWorstAndMakeGraphic();
}

void Solver::iteration() {
    updateWorstAngle();
    updateWorstDistance();
    updateWorstOverlap();
}

void Solver::setTargetSize(Vec2u targetSize) {
    this->targetSize = targetSize;
}

void Solver::updateDistances() {
    if (graphicData.distance <= 0.0) return;
    auto& n1 = nodes[idx1];
    auto& n2 = nodes[idx2];
    const auto diff = n2.position - n1.position;
    const auto dist = std::hypot(diff.x(), diff.y());
    const auto r12 = n1.radius + n2.radius;
    const auto transform = diff.normalized() * (dist - r12) * distanceFactor;
    n1.position += transform;
    n2.position -= transform;
}

void Solver::updateWorstOverlap() {
    if (graphicData.overlap <= 0.0) return;
    const auto [idx1, idx2] = graphicData.worstOverlap;
    auto& n1 = nodes[idx1];
    auto& n2 = nodes[idx2];
    const auto diff = n2.position - n1.position;
    const auto dist = std::hypot(diff.x(), diff.y());
    const auto r12 = n1.radius + n2.radius;
    const auto transform = diff.normalized() * (r12 - dist) * overlapFactor;
    n1.position -= transform;
    n2.position += transform;
}

void Solver::updateAngles() {
    if (graphicData.angle <= 0.0) return;
    const auto& o1 = original[idx1];
    const auto& o2 = original[idx2];
    auto& n1 = nodes[idx1];
    auto& n2 = nodes[idx2];
    const auto oDiff = o2.position - o1.position;
    const auto nDiff = n2.position - n1.position;
    auto angle = nDiff.angleTo(oDiff);
    if (angle > std::numbers::pi) {
        angle = - (std::numbers::pi * 2.0 - angle);
    }
    const auto transform = (nDiff.rotated2d(-angle * angleFactor) - nDiff) / 2.;
    n1.position += transform;
    n2.position -= transform;
}

void Solver::bestRotation() {
    double minAngle = std::numbers::pi;
    double maxAngle = -std::numbers::pi;
    for (int i = 0; i < nodes.size()-1; i++) {
        for (int j = i+1; j < nodes.size(); j++) {
            if (!edges[i][j]) continue;

            const auto& o1 = original[i];
            const auto& o2 = original[j];
            const auto& n1 = nodes[i];
            const auto& n2 = nodes[j];
            const auto nDiff = n2.position - n1.position;
            const auto oDiff = o2.position - o1.position;

            double angle = nDiff.angleTo(oDiff);
            if (angle > std::numbers::pi) {
                angle = -(std::numbers::pi * 2.0 - angle);
            }
            minAngle = std::min(minAngle, angle);
            maxAngle = std::max(maxAngle, angle);
        }
    }

    const double bestAngle = (minAngle + maxAngle) / 2.;
    for (auto& node : nodes) {
        node.position.rotate2d(bestAngle);
    }

    calculateWorstAndMakeGraphic();
}

void Solver::strengthenDistance() {
    distanceFactor = std::min(0.2, distanceFactor * 1.5 + 0.0001);
    calculateWorstAndMakeGraphic();
}

void Solver::weakenDistance() {
    distanceFactor = std::max(0.0, distanceFactor * 0.6 - 0.0001);
    calculateWorstAndMakeGraphic();
}

void Solver::strengthenOverlap() {
    overlapFactor = std::min(0.2, overlapFactor * 1.5 + 0.0001);
    calculateWorstAndMakeGraphic();
}

void Solver::weakenOverlap() {
    overlapFactor = std::max(0.0, overlapFactor * 0.6 - 0.0001);
    calculateWorstAndMakeGraphic();
}

void Solver::strengthenAngle() {
    angleFactor = std::min(1.0, angleFactor * 1.5 + 0.0001);
    calculateWorstAndMakeGraphic();
}

void Solver::weakenAngle() {
    angleFactor = std::max(0.0, angleFactor * 0.6 - 0.0001);
    calculateWorstAndMakeGraphic();
}

void Solver::increaseZoom() {
    zoom *= 0.9;
    calculateWorstAndMakeGraphic();
}

void Solver::decreaseZoom() {
    zoom *= 1.1;
    calculateWorstAndMakeGraphic();
}

void Solver::moveOffset(Vec2d move) {
    offset += move * zoom;
    calculateWorstAndMakeGraphic();
}

void Solver::printToFile() {
    OutputPrinter::printToFile(std::string(MD_OUTPUT_DIR) + "/" + filename + ".out", nodes);
    TET_INFO("Printed {}", graphicData.score);
}

void Solver::randomizePositions() {
    double maxRadius = 0.0;
    for (const auto& node : nodes) {
        maxRadius = std::max(maxRadius, node.radius);
    }

    std::random_device rd{};
    std::mt19937 rng{rd()};
    std::uniform_real_distribution<double> distr(0.0, std::sqrt(nodes.size()) * maxRadius);

    for (auto& node : nodes) {
        node.position = {distr(rng), distr(rng)};
    }

    calculateWorstAndMakeGraphic();
}
