#include "Solver.h"
#include "utils/log/Logger.h"

#include <algorithm>
#include <utils/reader/InputReader.h>
#include <utils/reader/OutputReader.h>
#include <utils/printer/OutputPrinter.h>

bool Solver::init(const std::string& filename) {
    this->filename = filename;
    auto inputData = InputReader::readFromFile(std::string(MD_INPUT_DIR) + '/' + filename);
    auto outputData = OutputReader::readFromFile(std::string(MD_OUTPUT_DIR) + '/' + filename + ".out");

    maxScore = calculateScore(inputData->nodes, *outputData, inputData->edges);

    original.clear();
    nodes.clear();
    edges.clear();

    overlapFactor = 0.1;
    distanceFactor = 0.1;
    angleFactor = 0.01;
    TET_INFO("Dist {} / Over {} / Angle {}", distanceFactor, overlapFactor, angleFactor);

    edgeInputCount = inputData->edges.size();

    for (const Node& node : inputData->nodes) {
        original.emplace_back(Node(node));
        nodes.emplace_back(Node(node));
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
    double scaleFactor = std::min(1 / (maxX - minX) * targetSize.x(), 1 / (maxY - minY) * targetSize.y());

    graphicData = FullGraphicData();
    for (const auto& circle : nodes) {
        auto newCircle = circle;
        newCircle.position -= positionOffset;
        newCircle.position.x() *= scaleFactor * zoom;
        newCircle.position.y() *= scaleFactor * zoom;
        newCircle.radius *= scaleFactor * zoom;
        newCircle.position -= offset;
        graphicData.circles.emplace_back(newCircle);
    }

    for (int i = 0; i < nodes.size() - 1; i++) {
        for (int j = i + 1; j < nodes.size(); j++) {
            bool touching = false;
            const auto& node1 = graphicData.circles[i];
            const auto& node2 = graphicData.circles[j];
            const auto diff = node1.position - node2.position;
            const auto dist = std::hypot(diff.x(), diff.y());
            const auto r12 = node1.radius + node2.radius;
            if (dist < r12) {
                touching = true;
                const auto overlap = (r12 - dist) / r12;
                if (overlap > graphicData.maxOverlap) {
                    graphicData.worstOverlap = {i, j};
                    graphicData.maxOverlap = overlap;
                }
            }
            if (!edges[i][j]) continue;
            const auto distance = (dist - r12) / r12;
            if (distance > graphicData.maxDistance) {
                graphicData.worstDistance = {i, j};
                graphicData.maxDistance = distance;
            }

            const auto in1 = original[i];
            const auto in2 = original[j];
            const auto inDiff = in1.position - in2.position;
            const auto angle = inDiff.smallestAngleTo(diff);
            if (angle > graphicData.maxAngle) {
                graphicData.worstAngle = {i, j};
                graphicData.maxAngle = angle;
            }

            auto edge = GraphicEdge();
            edge.touching = touching;
            edge.idxs = {i, j};
            graphicData.edges.emplace_back(edge);
        }
    }

    graphicData.maxAngle = radToDeg(graphicData.maxAngle);
    graphicData.maxOverlap *= 100.0;
    graphicData.maxDistance *= 100.0;

    graphicData.score = (1000 * (nodes.size() + edgeInputCount))
                        / (1 + 2 * graphicData.maxOverlap + graphicData.maxDistance + graphicData.maxAngle / 18);

    if (maxScore < graphicData.score) {
        maxScore = graphicData.score;
        TET_INFO("Print");
    }
    graphicData.maxScore = maxScore;
}

void Solver::run(unsigned iterations) {
    for (int i = 0; i < iterations; i++) {
        iteration();
        calculateWorstAndMakeGraphic();
    }
    bestRotation();
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

void Solver::updateWorstDistance() {
    if (graphicData.maxDistance <= 0.0) return;
    const auto [idx1, idx2] = graphicData.worstDistance;
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
    if (graphicData.maxOverlap <= 0.0) return;
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

void Solver::updateWorstAngle() {
    if (graphicData.maxAngle <= 0.0) return;
    const auto [idx1, idx2] = graphicData.worstAngle;
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
    TET_INFO("Dist {} / Over {} / Angle {}", distanceFactor, overlapFactor, angleFactor);
}

void Solver::weakenDistance() {
    distanceFactor = std::max(0.0, distanceFactor * 0.6 - 0.0001);
    TET_INFO("Dist {} / Over {} / Angle {}", distanceFactor, overlapFactor, angleFactor);
}

void Solver::strengthenOverlap() {
    overlapFactor = std::min(0.2, overlapFactor * 1.5 + 0.0001);
    TET_INFO("Dist {} / Over {} / Angle {}", distanceFactor, overlapFactor, angleFactor);
}

void Solver::weakenOverlap() {
    overlapFactor = std::max(0.0, overlapFactor * 0.6 - 0.0001);
    TET_INFO("Dist {} / Over {} / Angle {}", distanceFactor, overlapFactor, angleFactor);
}

void Solver::strengthenAngle() {
    angleFactor = std::min(1.0, angleFactor * 1.5 + 0.0001);
    TET_INFO("Dist {} / Over {} / Angle {}", distanceFactor, overlapFactor, angleFactor);
}

void Solver::weakenAngle() {
    angleFactor = std::max(0.0, angleFactor * 0.6 - 0.0001);
    TET_INFO("Dist {} / Over {} / Angle {}", distanceFactor, overlapFactor, angleFactor);
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
