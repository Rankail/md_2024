#include "Solver.h"

#include <algorithm>
#include <ranges>

bool Solver::init(const FullInputData* inputData) {

    original.clear();
    nodes.clear();
    edges.clear();

    for (const Node* node : inputData->nodes) {
        original.emplace_back(*node);
        nodes.emplace_back(*node);
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
                auto angle = diff.angleTo(orgDiff);
                if (angle > std::numbers::pi) angle -= 2 * std::numbers::pi;
                const auto rotateBy = angle * 0.02;
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

void Solver::printScore() {
    double maxDistance = 0.0;
    double maxOverlap = 0.0;
    double maxAngle = 0.0;

    for (int i = 0; i < nodes.size()-1; i++) {
        const auto& n1 = nodes[i];
        for (int j = i + 1; j < nodes.size(); j++) {
            const auto& n2 = nodes[j];
            const auto diff = n1.position - n2.position;
            const auto a2b2 = diff.x() * diff.x() + diff.y() * diff.y();
            const auto r12 = n1.radius + n2.radius;
            const auto c2 = r12 * r12;
            if (a2b2 < c2) {
                const auto dist = std::sqrt(a2b2);
                const auto overlap = (r12 - dist) / r12;
                maxOverlap = std::max(maxOverlap, overlap);
            }
            if (edges[i][j]) {
                if (a2b2 > c2) {
                    const auto dist = std::sqrt(a2b2);
                    const auto unwantedDist = (dist - r12) / r12;
                    maxDistance = std::max(maxDistance, unwantedDist);
                }

                const auto in1 = original[i];
                const auto in2 = original[j];
                const auto inDiff = in1.position - in2.position;
                const auto angle = radToDeg(inDiff.smallestAngleTo(diff));
                maxAngle = std::max(maxAngle, (double)angle);
            }
        }
    }

    const auto overlapFactor = maxOverlap * 200;
    const auto distanceFactor = maxDistance * 100;
    const auto angleFactor = maxAngle / 18.0;

    const auto score = 1000.0 * (nodes.size() + edges.size()) / (1 + overlapFactor + distanceFactor + angleFactor);
    const auto scoreRounded = (int)(score + 0.5);

    std::cout   << "Max overlap: " << maxOverlap << std::endl
                << "Max distance: " << maxDistance << std::endl
                << "Max angle: " << maxAngle << std::endl
                << "Score: " << scoreRounded << std::endl;
}
