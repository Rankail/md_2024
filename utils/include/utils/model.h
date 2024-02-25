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
    std::pair<Vec2d, Vec2d> line;
};

struct FullGraphicData {
    std::vector<Node> circles;
    std::vector<GraphicEdge> lines;
    unsigned worstOverlap = -1;
    unsigned worstDistance = -1;
    unsigned worstAngle = -1;
};

#endif //MD_2024_MODEL_H
