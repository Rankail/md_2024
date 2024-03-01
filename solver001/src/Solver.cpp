#include "Solver.h"

#include <algorithm>

bool Solver::init(const FullInputData* inputData) {
    for (const Node& node : inputData->nodes) {
        original.push_back(node);
        nodes.push_back(node);
    }

    edges = std::vector<std::vector<bool>>(nodes.size(), std::vector<bool>(nodes.size(), false));

    for (const auto [idx1, idx2] : inputData->edges) {
        edges[idx1][idx2] = true;
        edges[idx2][idx1] = true;
    }

    return true;
}

void Solver::run(unsigned iterations) {
    for (int i = 0; i < iterations; i++) {
        iteration();
    }
}

void Solver::printResult() {

}

void Solver::iteration() {
    auto forces = std::vector<Vec2d>(nodes.size(), {0.0, 0.0});
    for (int i = 0; i < nodes.size() - 1; i++) {
        const auto& n1 = nodes[i];
        for (int j = i + 1; j < nodes.size(); j++) {
            const auto& n2 = nodes[j];
            const auto diff = n2.position - n1.position;
            const auto a2b2 = diff.x() * diff.x() + diff.y() * diff.y();
            const auto r12 = n1.radius + n2.radius;
            const auto c2 = r12 * r12;
            if(edges[i][j]) {
                Vec2d orgDiff = original[j].position - original[i].position;
                const auto angle = diff.angleTo(orgDiff);
                const auto rotateBy = -angle * 0.001;
                const auto n2Rot = (diff / 2.).rotated2d(rotateBy);
                const auto n2Trans = n2Rot - (diff / 2.);
                forces[j] += n2Trans;
                forces[i] -= n2Trans;
            }
            if (a2b2 < c2) {
                const auto dist = std::sqrt(a2b2);
                const auto force = (r12 - dist) / r12 * diff * 0.1;
                forces[j] += force;
                forces[i] -= force;
                continue;
            }
            if (!edges[i][j]) continue;
            if (a2b2 > c2) {
                const auto dist = std::sqrt(a2b2);
                const auto force = (dist - r12) / dist * diff * 0.1;
                forces[j] -= force;
                forces[i] += force;
            }
        }
    }

    for (int i = 0; i < nodes.size(); i++) {
        nodes[i].position += forces[i];
    }
}
