#ifndef MD_2024_MODEL_H
#define MD_2024_MODEL_H

#include <string>
#include <vector>

#include "vec.h"

struct Node {
    unsigned index;
    std::string name;
    double weight;
    Vec2d originalPosition;
};

struct FullInputData {
    std::vector<Node*> nodes;
    std::vector<std::pair<unsigned, unsigned>> edges;
};

#endif //MD_2024_MODEL_H
