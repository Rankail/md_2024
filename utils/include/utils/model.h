#ifndef MD_2024_MODEL_H
#define MD_2024_MODEL_H

#include <string>
#include <vector>

#include "math/vec.h"
#include "utils.h"

// -- basic --

struct Node {
    unsigned index;
    std::string name;
    double radius;
    Vec2d position;
};

struct FullInputData {
    std::vector<Node> nodes;
    std::vector<uPair> edges;
};

// -- graphic --

struct GraphicEdge {
    bool touching = false;
    uPair idxs;
};

struct FullGraphicData {
    std::vector<Node> circles;
    std::vector<GraphicEdge> edges;

    double overlap = 0.0;
    double distance = 0.0;
    double angle = 0.0;

    int score = -1.0;
    int maxScore = 0.0;

    double distanceFactor = 0.1;
    double overlapFactor = 0.1;
    double angleFactor = 0.1;

    unsigned totalSteps = 0;
};

struct FullGraphicDataWorstEdges : public FullGraphicData {
    uPair worstOverlap = {-1, -1};
    uPair worstDistance = {-1, -1};
    uPair worstAngle = {-1, -1};
};

struct FullGraphicData2  : public FullGraphicData {
    uPair worstOverlap = {-1, -1};
};

// -- polygon --

struct PolygonNode;

struct PolygonEdge {
    double angle;
    double originalAngle;
    std::shared_ptr<PolygonNode> a;
    std::shared_ptr<PolygonNode> b;

    static std::shared_ptr<PolygonEdge> fromNodes(std::shared_ptr<PolygonNode> a, std::shared_ptr<PolygonNode> b);
};

struct PolygonNode : public Node {
    std::vector<std::shared_ptr<PolygonEdge>> edges{}; // ordered clockwise

    PolygonNode(const Node& node)
        : Node(node), edges{} {}
};

struct Polygon {
    std::vector<std::shared_ptr<PolygonEdge>> edges{}; // clockwise
};

// -- helper --

int calculateScore(const std::vector<Node>& original, const std::vector<Node>& nodes, const std::vector<uPair>& edges);
int calculateScore2(const std::vector<Node>& original, const std::vector<Node>& nodes, const std::vector<uPair>& edges);

#endif //MD_2024_MODEL_H
