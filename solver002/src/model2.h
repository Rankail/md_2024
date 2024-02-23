#ifndef MD_2024_MODEL2_H
#define MD_2024_MODEL2_H

#include <utils/math/vec.h>
#include <vector>

struct GraphNode {
    unsigned id;
    double radius;
    Vec2d orgPos;
    Vec2d pos;
    bool visited = false;
    std::vector<GraphNode*> neigbors{};
};

#endif //MD_2024_MODEL2_H
