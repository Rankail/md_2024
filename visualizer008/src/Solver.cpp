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

    stepSize = 1.0;

    original.clear();
    nodes.clear();
    edges.clear();
    angleGradients.clear();
    distGradients.clear();
    overlapGradients.clear();

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

    graphicData = FullGraphicData();
    makeGraphic();

    return true;
}

void Solver::makeGraphic(bool updateScore) {
    auto minX = std::numeric_limits<double>::max();
    auto minY = std::numeric_limits<double>::max();
    auto maxX = std::numeric_limits<double>::min();
    auto maxY = std::numeric_limits<double>::min();

    // bounding rect
    for (const auto& circle: nodes) {
        minX = std::min(minX, circle.position.x() - circle.radius);
        minY = std::min(minY, circle.position.y() - circle.radius);
        maxX = std::max(maxX, circle.position.x() + circle.radius);
        maxY = std::max(maxY, circle.position.y() + circle.radius);
    }
    Vec2d positionOffset = Vec2d(minX, minY);
    double scaleFactor = std::min(targetSize.x() / (maxX - minX), targetSize.y() / (maxY - minY));

    // camera
    graphicData.circles.clear();
    graphicData.edges.clear();
    for (const auto& circle: nodes) {
        auto newCircle = circle;
        newCircle.position -= positionOffset;
        newCircle.position.x() *= scaleFactor * zoom;
        newCircle.position.y() *= scaleFactor * zoom;
        newCircle.radius *= scaleFactor * zoom;
        newCircle.position -= offset;
        graphicData.circles.emplace_back(newCircle);
    }

    for (unsigned i = 0; i < nodes.size()-1; i++) {
        for (unsigned j = i+1; j < nodes.size(); j++) {
            if (!edges[i][j]) continue;
            auto edge = GraphicEdge{};
            edge.idxs = {i, j};
            graphicData.edges.emplace_back(edge);
        }
    }

    graphicData.stepSize = stepSize;

    if (updateScore)
        calculateScore();
}

void Solver::calculateScore() {
    // score
    double maxOverlap = 0.0;
    double distanceSum = 0.0;
    double angleSum = 0.0;
    unsigned edgeCount = 0;

    for (int i = 0; i < nodes.size() - 1; i++) {
        for (int j = i + 1; j < nodes.size(); j++) {
            edgeCount++;
            const auto& node1 = nodes[i];
            const auto& node2 = nodes[j];
            const auto diff = node1.position - node2.position;
            const auto dist = std::hypot(diff.x(), diff.y());
            const auto r12 = node1.radius + node2.radius;
            if (dist < r12) {
                const auto overlap = (r12 - dist) / r12;
                if (overlap > maxOverlap) {
                    maxOverlap = overlap;
                }
            }

            if (!edges[i][j]) continue;

            if (dist > r12) {
                const auto distance = (dist - r12) / r12;
                distanceSum += distance;
            }

            const auto& original1 = original[i];
            const auto& original2 = original[j];
            const auto diff2 = original1.position - original2.position;
            const auto angle = diff.smallestAngleTo(diff2);
            angleSum += angle / std::numbers::pi;
        }
    }

    maxOverlap *= 100.0;
    const auto angleAvg = angleSum * 100.0 / edgeInputCount;
    const auto distanceAvg = distanceSum * 100.0 / edgeInputCount;

    const auto overlapFactor = maxOverlap * maxOverlap * 0.1;
    const auto distanceFactor = distanceAvg * distanceAvg * 0.05;
    const auto angleFactor = angleAvg * angleAvg * 0.05;

    graphicData.score = (1000 * (nodes.size() + edgeInputCount))
                        / (1 + overlapFactor + distanceFactor + angleFactor);

    graphicData.overlap = maxOverlap;
    graphicData.distance = distanceAvg;
    graphicData.angle = angleAvg;

    if (maxScore < graphicData.score) {
        maxScore = graphicData.score;
        printToFile();
    }
    graphicData.maxScore = maxScore;

    graphicData.totalSteps = totalSteps;
}

void Solver::run(unsigned iterations) {
    for (int i = 0; i < iterations; i++) {
        iteration();
    }
    bestRotation();
    makeGraphic();
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

    makeGraphic();
}

void Solver::iteration() {
    angleGradients = std::vector<Vec2d>(nodes.size(), {0.0, 0.0});
    distGradients = std::vector<Vec2d>(nodes.size(), {0.0, 0.0});
    overlapGradients = std::vector<Vec2d>(nodes.size(), {0.0, 0.0});

    calculateOverlapDerivative();
    for (const auto& node : nodes) {
        calculateAngleDerivatives(node);
        calculateDistDerivatives(node);
    }

    applyGradients(overlapGradients, stepSize);
    applyGradients(angleGradients, stepSize);
    applyGradients(distGradients, stepSize);


    // calculate derivative for every parameter
        // derivative of dist

        // derivative of angle
        // derivative of overlap?
            // try with average => abs of dist
            // ignore if not maximum
            // only activate if close to maximum


}

void Solver::setTargetSize(Vec2u targetSize) {
    this->targetSize = targetSize;
}

void Solver::bestRotation() {
    // TODO adjust for average
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

    makeGraphic();
}

void Solver::increaseZoom() {
    zoom *= 0.9;
    makeGraphic(false);
}

void Solver::decreaseZoom() {
    zoom *= 1.1;
    makeGraphic(false);
}

void Solver::moveOffset(Vec2d move) {
    offset += move * zoom;
    makeGraphic(false);
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

    makeGraphic();
}

void Solver::calculateOverlapDerivative() {
    uPair overlapIndices = {-1, -1};
    double maxOverlap = 0.0;
    for (int i = 0; i < nodes.size() - 1; i++) {
        for (int j = i + 1; j < nodes.size(); j++) {
            const auto& n1 = nodes[i];
            const auto& n2 = nodes[j];
            const auto diff = n2.position - n1.position;
            const auto dist = std::hypot(diff.x(), diff.y());
            const auto r12 = n1.radius + n2.radius;

            if (dist > r12) continue;

            double overlap = (r12 - dist) / r12;
            if (overlap > maxOverlap) {
                maxOverlap = overlap;
                overlapIndices = {i, j};
            }
        }
    }

    if (overlapIndices.first == -1) return;

    const auto& n1 = nodes[overlapIndices.first];
    const auto& n2 = nodes[overlapIndices.second];
    const auto diff = n2.position - n1.position;
    const auto dist = std::hypot(diff.x(), diff.y());
    // derivative for x
    double dx = (n2.position.x() - n1.position.x()) /
                (n1.radius + n2.radius) * dist;
    // derivative for y
    double dy = (n1.position.y() - n2.position.y()) /
                (n1.radius + n2.radius) * dist;

    Vec2d gradient{dx, dy};
    gradient *= 100;
    gradient *= 0.1;

    overlapGradients[overlapIndices.first] += gradient;
    overlapGradients[overlapIndices.second] -= gradient;
}

void Solver::calculateDistDerivatives(const Node& node) {
    Vec2d gradient{0.0, 0.0};
    for (int i = 0; i < nodes.size(); i++) {
        if (!edges[node.index][i]) continue;

        const auto diff = nodes[i].position - node.position;
        const auto dist = std::hypot(diff.x(), diff.y());
        if (dist < node.radius + nodes[i].radius) continue;
        // derivative for x
        double dx = (node.position.x() - nodes[i].position.x()) /
                   (node.radius + nodes[i].radius) * dist;
        // derivative for y
        double dy = (node.position.y() - nodes[i].position.y()) /
                    (node.radius + nodes[i].radius) * dist;

        gradient += Vec2d{dx, dy};
    }

    gradient *= 100.0 / edgeInputCount;
    gradient *= 0.05;

    distGradients[node.index] -= gradient;

}

void Solver::calculateAngleDerivatives(const Node &n1) {
    Vec2d gradientSum = {0.0,0.0};
    for (int i = 0; i < nodes.size() - 1; i++) {
        if (i == n1.index || !edges[n1.index][i]) continue;

        const auto& n2 = nodes[i];
        const auto& o1 = original[n1.index];
        const auto& o2 = original[i];

        const auto nDiff = n2.position - n1.position;
        const auto oDiff = o2.position - o1.position;

        auto gradient = calculateAngleDerivative(nDiff, oDiff);

        gradientSum += gradient;
    }

    gradientSum *= 100.0 / std::numbers::pi / edgeInputCount;
    gradientSum *= 0.05;

    angleGradients[n1.index] += gradientSum;
}

Vec2d Solver::calculateAngleDerivative(Vec2d nDiff, Vec2d oDiff) {
    const auto cx = nDiff.x();
    const auto cy = nDiff.y();
    const auto ox = oDiff.x();
    const auto oy = oDiff.y();
    const auto cLenSqr = cx * cx + cy * cy;
    const auto oLenSqr = ox * ox + oy * oy;
    const auto scalar = (cx*ox + cy*oy);

    // derivative of smallest angle between two vectors
    double dx = (cx * (cx * ox + cy * oy))
        / (
            std::pow(cLenSqr * oLenSqr, 1.5)
            * std::sqrt(oLenSqr)
            * std::sqrt(
                1 - std::pow(cy * ox - cx * oy, 2.0) / (cLenSqr * oLenSqr)
            )
        );
    double dy = (cx * (cx * ox + cy * oy))
        / (
            std::pow(cLenSqr * oLenSqr, 1.5)
            * std::sqrt(oLenSqr)
            * std::sqrt(
                1 - std::pow(cy * ox - cx * oy, 2.0) / (cLenSqr * oLenSqr)
            )
        );
//    double dx = (
//        (
//            cx * oLenSqr * scalar / std::pow(cLenSqr * oLenSqr, 1.5)
//        ) - (
//            ox / std::sqrt(cLenSqr * oLenSqr)
//        )
//    ) / (
//        std::sqrt(1 - (scalar * scalar) / (cLenSqr * oLenSqr))
//    );

//    double dy = (
//        (
//            cy * oLenSqr * scalar / std::pow(cLenSqr * oLenSqr, 1.5)
//        ) - (
//            oy / std::sqrt(cLenSqr * oLenSqr)
//        )
//    ) / (
//        std::sqrt(1 - (scalar * scalar) / (cLenSqr * oLenSqr))
//    );

    if (std::isnan(dx) || std::isinf(dx)) dx = 0.0;
    if (std::isnan(dy) || std::isinf(dy)) dy = 0.0;

    return {dx, dy};
}

void Solver::applyGradients(const std::vector<Vec2d> &gradients, double stepSize) {
    Vec2d maximum = Vec2d{0.0, 0.0};
    for (const auto& g : gradients) {
        maximum.x() = std::max(maximum.x(), g.x());
        maximum.y() = std::max(maximum.y(), g.y());
    }

    if (maximum.x() == 0.0 || maximum.y() == 0.0) return;

    for (int i = 0; i < nodes.size(); i++) {
        nodes[i].position += stepSize * gradients[i] / maximum;
    }
}

void Solver::increaseStepSize() {
    stepSize *= 1.5;
}

void Solver::decreaseStepSize() {
    stepSize *= 2. / 3.;
}
