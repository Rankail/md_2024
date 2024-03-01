#include "utils/model.h"

#include <ranges>

int calculateScore(const std::vector<Node>& original, const std::vector<Node>& nodes, const std::vector<uPair>& edges) {
    auto edgeMat = std::vector<std::vector<bool>>(nodes.size(), std::vector<bool>(nodes.size(), false));
    for (const auto [idx1, idx2] : edges) {
        edgeMat[idx1][idx2] = true;
        edgeMat[idx2][idx1] = true;
    }

    unsigned edgeIdx = 0;
    double maxDistance = 0.0;
    double maxOverlap = 0.0;
    double maxAngle = 0.0;

    for (int i = 0; i < nodes.size()-1; i++) {
        for (int j = i + 1; j < nodes.size(); j++) {
            const auto& n1 = nodes[i];
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
            if (edgeMat[i][j]) {
                if (a2b2 > c2) {
                    const auto dist = std::sqrt(a2b2);
                    const auto unwantedDist = (dist - r12) / r12;
                    maxDistance = std::max(maxDistance, unwantedDist);
                }

                const auto& in1 = original[i];
                const auto& in2 = original[j];
                const auto inDiff = in1.position - in2.position;
                const auto angle = radToDeg(inDiff.smallestAngleTo(diff));
                maxAngle = std::max(maxAngle, (double)angle);
                edgeIdx++;
            }
        }
    }

    const auto overlapFactor = maxOverlap * 200;
    const auto distanceFactor = maxDistance * 100;
    const auto angleFactor = maxAngle / 18.0;

    const auto score = 1000.0 * (nodes.size() + edges.size()) / (1 + overlapFactor + distanceFactor + angleFactor);
    const auto scoreRounded = (int)(score + 0.5);

    return scoreRounded;
}