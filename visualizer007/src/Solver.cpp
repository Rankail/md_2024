#include "Solver.h"
#include "utils/log/Logger.h"

#include <algorithm>
#include <random>
#include <unordered_set>

#include <utils/reader/InputReader.h>
#include <utils/reader/OutputReader.h>
#include <utils/printer/OutputPrinter.h>

template<typename T>
concept Hashable = requires {
    typename std::hash<T>;
};

struct PairHash {
    template <class T1, class T2> requires Hashable<T1> && Hashable<T2>
    std::size_t operator () (const std::pair<T1,T2> &p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        return h1 ^ h2; // not good if (first == second)
    }
};

bool Solver::init(const std::string& filename) {
    this->filename = filename;
    auto inputData = InputReader::readFromFile(std::string(MD_INPUT_DIR) + '/' + filename);
    auto outputData = OutputReader::readFromFile(std::string(MD_OUTPUT_DIR) + '/' + filename + ".out");

    maxScore = calculateScore2(inputData->nodes, *outputData, inputData->edges);

    totalSteps = 0;

    edgeInputCount = inputData->edges.size();

    for (auto& node : nodes) delete node;
    nodes.clear();


    for (const Node& node : inputData->nodes) {
        nodes.push_back(new PolygonNode(node));
    }

    // create edges
    for (auto& polygon : polygons) delete polygon;
    polygons.clear();

    for (auto& edge : edges) delete edge;
    edgeMatrix = std::vector<std::vector<bool>>(nodes.size(), std::vector<bool>(nodes.size(), false));

    std::unordered_set<std::shared_ptr<PolygonEdge>> edgeSet{}; // for polygon-construction

    for (const auto [idx1, idx2] : inputData->edges) {
        if (edgeMatrix[idx1][idx2]) continue;
        edgeMatrix[idx1][idx2] = true;
        edgeMatrix[idx2][idx1] = true;

        auto& n1 = nodes[idx1];
        auto& n2 = nodes[idx2];
        auto edge1 = PolygonEdge::fromNodes(n1, n2);
        auto edge2 = PolygonEdge::fromNodes(n2, n1);

        n1->edges.emplace_back(edge1);
        n2->edges.emplace_back(edge2);
        edges.emplace_back(edge1);
        edges.emplace_back(edge2);
    }

    // sort edges per node
    for (auto& node : nodes) {
        std::sort(node->edges.begin(), node->edges.end(), [](const PolygonEdge* a, const PolygonEdge* b) {
            return a->angle > b->angle;
        });
    }

    // create polygons
    createPolygons();

    makeGraphic();

    return true;
}

void Solver::createPolygons() {
    auto edgeSet = std::unordered_set<PolygonEdge*>(edges.begin(), edges.end());
    while (!edgeSet.empty()) {
        auto randomEdge = edgeSet.begin();
        while (randomEdge != edgeSet.end() && (*randomEdge)->a->edges.size() <= 1)
            randomEdge++;
        if (randomEdge == edgeSet.end()) {
            TET_CRITICAL("Only nodes with a single edge are left");
        }

        auto newPolygon = createPolygon(*edgeSet.begin());

        for (auto& edge : newPolygon->edges) {
            edgeSet.erase(edge);
        }

        polygons.emplace_back(newPolygon);
    }
}

PolygonShape* Solver::createPolygon(PolygonEdge* startEdge) {
    PolygonEdge* currentEdge = startEdge;
    auto polygon = new PolygonShape();
    polygon->edges.emplace_back(startEdge);
    TET_INFO("Starting Polygon:");
    TET_INFO("{}->{}", currentEdge->a->name, currentEdge->b->name);
    int i = 0;
    while (i < currentEdge->b->edges.size()) {
        if (currentEdge->b->edges[i]->b->index == currentEdge->a->index) {
            // rotate to next edge
            currentEdge = currentEdge->b->edges[(i + 1) % currentEdge->b->edges.size()];
            polygon->edges.emplace_back(currentEdge);
            TET_INFO("{}->{}", currentEdge->a->name, currentEdge->b->name);

            // is end?
            if (currentEdge->b->index == startEdge->a->index) {
                return polygon;
            }

            i = 0;
            continue;
        }
        i++;
    }
    TET_CRITICAL("Found no edge {}->{}", currentEdge->b->name, currentEdge->a->name);
}

void Solver::makeGraphic(bool updateScore) {
    auto minX = std::numeric_limits<double>::max();
    auto minY = std::numeric_limits<double>::max();
    auto maxX = std::numeric_limits<double>::min();
    auto maxY = std::numeric_limits<double>::min();

    // bounding rect
    for (const auto& circle: nodes) {
        minX = std::min(minX, circle->position.x() - circle->radius);
        minY = std::min(minY, circle->position.y() - circle->radius);
        maxX = std::max(maxX, circle->position.x() + circle->radius);
        maxY = std::max(maxY, circle->position.y() + circle->radius);
    }
    Vec2d positionOffset = Vec2d(minX, minY);
    double scaleFactor = std::min(targetSize.x() / (maxX - minX), targetSize.y() / (maxY - minY));

    // camera
    graphicData = FullGraphicData();
    for (const auto& circle: nodes) {
        auto newCircle = circle;
        newCircle->position -= positionOffset;
        newCircle->position.x() *= scaleFactor * zoom;
        newCircle->position.y() *= scaleFactor * zoom;
        newCircle->radius *= scaleFactor * zoom;
        newCircle->position -= offset;
        graphicData.circles.emplace_back(*newCircle);
    }

    for (const auto& edge : edges) {
        auto gEdge = GraphicEdge();
        gEdge.idxs = {edge->a->index, edge->b->index};
        graphicData.edges.emplace_back(gEdge);
    }

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
            const auto& node1 = graphicData.circles[i];
            const auto& node2 = graphicData.circles[j];
            const auto diff = node1.position - node2.position;
            const auto dist = std::hypot(diff.x(), diff.y());
            const auto r12 = node1.radius + node2.radius;
            if (dist < r12) {
                const auto overlap = (r12 - dist) / r12;
                if (overlap > maxOverlap) {
                    maxOverlap = overlap;
                }
            }
        }
    }

    for (const auto& edge : edges) {
        const auto& node1 = edge->a;
        const auto& node2 = edge->b;
        const auto diff = node1->position - node2->position;
        const auto dist = std::hypot(diff.x(), diff.y());
        const auto r12 = node1->radius + node2->radius;

        if (dist > r12) {
            const auto distance = (dist - r12) / r12;
            distanceSum += distance;
        }

        const auto angle = std::abs(edge->angle - edge->originalAngle);
        angleSum += angle / std::numbers::pi;

        auto gEdge = GraphicEdge();
        gEdge.idxs = {node1->index, node2->index};
        graphicData.edges.emplace_back(gEdge);
    }

    maxOverlap *= 100.0;
    const auto angleAvg = angleSum * 100.0 / edges.size();
    const auto distanceAvg = distanceSum * 100.0 / edges.size();

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
        makeGraphic();
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
            const auto diff = n1->position - n2->position;
            const auto dist = std::hypot(diff.x(), diff.y());
            const auto r12 = n1->radius + n2->radius;
            const auto overlap = (r12 - dist) / dist;
            overlaps.emplace_back(overlap);
        }
    }

    std::sort(overlaps.begin(), overlaps.end());
    const auto smallOverlap = overlaps[overlaps.size() * 99 / 100];

    const auto factor = 1 + smallOverlap;
    for (int i = 0; i < nodes.size(); i++) {
        nodes[i]->position *= factor;
    }

    makeGraphic();
}

void Solver::iteration() {

}

void Solver::setTargetSize(Vec2u targetSize) {
    this->targetSize = targetSize;
}

//void Solver::updateDistances() {
//    if (graphicData.distance <= 0.0) return;
//    auto& n1 = nodes[idx1];
//    auto& n2 = nodes[idx2];
//    const auto diff = n2.position - n1.position;
//    const auto dist = std::hypot(diff.x(), diff.y());
//    const auto r12 = n1.radius + n2.radius;
//    const auto transform = diff.normalized() * (dist - r12) * distanceFactor;
//    n1.position += transform;
//    n2.position -= transform;
//}
//
//void Solver::updateWorstOverlap() {
//    if (graphicData.overlap <= 0.0) return;
//    const auto [idx1, idx2] = graphicData.worstOverlap;
//    auto& n1 = nodes[idx1];
//    auto& n2 = nodes[idx2];
//    const auto diff = n2.position - n1.position;
//    const auto dist = std::hypot(diff.x(), diff.y());
//    const auto r12 = n1.radius + n2.radius;
//    const auto transform = diff.normalized() * (r12 - dist) * overlapFactor;
//    n1.position -= transform;
//    n2.position += transform;
//}
//
//void Solver::updateAngles() {
//    if (graphicData.angle <= 0.0) return;
//    const auto& o1 = original[idx1];
//    const auto& o2 = original[idx2];
//    auto& n1 = nodes[idx1];
//    auto& n2 = nodes[idx2];
//    const auto oDiff = o2.position - o1.position;
//    const auto nDiff = n2.position - n1.position;
//    auto angle = nDiff.angleTo(oDiff);
//    if (angle > std::numbers::pi) {
//        angle = - (std::numbers::pi * 2.0 - angle);
//    }
//    const auto transform = (nDiff.rotated2d(-angle * angleFactor) - nDiff) / 2.;
//    n1.position += transform;
//    n2.position -= transform;
//}

void Solver::bestRotation() {
    double minAngle = std::numbers::pi;
    double maxAngle = -std::numbers::pi;
    for (const auto& edge : edges) {
        double angle = edge->angle - edge->originalAngle;
        minAngle = std::min(minAngle, angle);
        maxAngle = std::max(maxAngle, angle);
    }

    const double bestAngle = (minAngle + maxAngle) / 2.;
    for (auto& node : nodes) {
        node->position.rotate2d(bestAngle);
    }

    makeGraphic();
}

void Solver::increaseZoom() {
    zoom *= 0.9;
    makeGraphic();
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
    std::vector<Node> basicNodes{};
    for (const auto& node : nodes) {
        basicNodes.emplace_back(*node);
    }
    OutputPrinter::printToFile(std::string(MD_OUTPUT_DIR) + "/" + filename + ".out", basicNodes);
    TET_INFO("Printed {}", graphicData.score);
}

//void Solver::randomizePositions() {
//    double maxRadius = 0.0;
//    for (const auto& node : nodes) {
//        maxRadius = std::max(maxRadius, node.radius);
//    }
//
//    std::random_device rd{};
//    std::mt19937 rng{rd()};
//    std::uniform_real_distribution<double> distr(0.0, std::sqrt(nodes.size()) * maxRadius);
//
//    for (auto& node : nodes) {
//        node.position = {distr(rng), distr(rng)};
//    }
//
//    calculateWorstAndMakeGraphic();
//}
