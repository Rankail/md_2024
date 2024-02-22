#ifndef MD_2024_MODEL_H
#define MD_2024_MODEL_H

#include <string>
#include <vector>

#include "vec.h"
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

#endif //MD_2024_MODEL_H
