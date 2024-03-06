#ifndef MD_2024_MODEL_H
#define MD_2024_MODEL_H

#include <string>
#include <vector>

#include "math/vec.h"
#include "utils.h"

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

int calculateScore(const std::vector<Node>& original, const std::vector<Node>& nodes, const std::vector<uPair>& edges);
int calculateScore2(const std::vector<Node>& original, const std::vector<Node>& nodes, const std::vector<uPair>& edges);

#endif //MD_2024_MODEL_H
