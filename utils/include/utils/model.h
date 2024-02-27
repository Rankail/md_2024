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
    std::vector<Node*> nodes;
    std::vector<uPair> edges;
};

struct GraphicEdge {
    bool touching = false;
    uPair idxs;
};

struct FullGraphicData {
    std::vector<Node> circles;
    std::vector<GraphicEdge> edges;
    uPair worstOverlap = {-1, -1};
    uPair worstDistance = {-1, -1};
    uPair worstAngle = {-1, -1};
    double maxOverlap = 0.0;
    double maxDistance = 0.0;
    double maxAngle = 0.0;
    double score = -1.0;
};

#endif //MD_2024_MODEL_H
