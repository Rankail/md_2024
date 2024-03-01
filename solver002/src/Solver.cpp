#include "Solver.h"

#include <algorithm>

#include "model2.h"

bool Solver::init(const FullInputData* inputData) {
    for (const Node& node : inputData->nodes) {
        nodes.emplace_back(new GraphNode{
            node.index,
            node.radius,
            node.position,
            node.position
        });
    }

    for (const auto [idx1, idx2] : inputData->edges) {
        nodes[idx1]->neigbors.emplace_back(nodes[idx2]);
        nodes[idx2]->neigbors.emplace_back(nodes[idx1]);
    }

    for (GraphNode* node : nodes) {
        std::ranges::sort(node->neigbors, [&](const GraphNode* a, const GraphNode* b) {
            return (node->orgPos - a->orgPos).angleTo(node->pos - a->pos)
                < (node->orgPos - b->orgPos).angleTo(node->pos - b->pos);
        });
    }

    return true;
}

void Solver::run() {
    nodes[0]->visited = true;
    place(nodes[0]);
}

void Solver::place(GraphNode *current) {
    for (GraphNode* neigbor : current->neigbors) {
        if (neigbor->visited) continue;

        const auto len = (neigbor->orgPos - current->orgPos).norm();
        const auto newDiff = (neigbor->orgPos - current->orgPos) * (neigbor->radius + current->radius) / len;
        neigbor->pos = current->pos + newDiff;
        neigbor->visited = true;
        place(neigbor);
    }
}

std::vector<Node> Solver::getNodes(const std::vector<Node>& originalNodes) {
    std::vector<Node> newNodes{};
    for (GraphNode* node : nodes) {
        auto newNode = originalNodes[node->id];
        newNode.position = node->pos;
        newNodes.emplace_back(newNode);
    }

    return newNodes;
}
