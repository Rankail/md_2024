#include "atlas/polygon.h"

static bool isPolygonClockwise(const std::vector<Vec2f>& points) {
    auto maxY = points[0].y();
    int maxYIdx = 0;
    for (int i = 1; i < points.size(); ++i) {
        if (points[i].y() > maxY) {
            maxY = points[i].y();
            maxYIdx = i;
        }
    }
    if (maxYIdx == 0) { // special case if first point is part of a flat top
        while (maxYIdx < points.size()-1 && points[maxYIdx+1].y() == maxY) ++maxYIdx;
    }

    const auto& a = points[maxYIdx];
    const auto& b = points[(maxYIdx - 1 + points.size()) % points.size()];
    const auto& c = points[(maxYIdx + 1) % points.size()];

    const auto ab = b - a;
    const auto ac = c - a;

    return (ab.x() * ac.y() - ac.x() * ab.y()) < 0.f;
}

static bool isPointInTriangle(const Vec2f& p, const Vec2f& a, const Vec2f& b, const Vec2f& c) {
    const auto ap = p - a;

    const bool sideAB = (b.x() - a.x()) * ap.y() - (b.y() - a.y()) * ap.x() > 0; // left or right of ab

    if ((c.x() - a.x()) * ap.y() - (c.y() - a.y()) * ap.x() > 0 == sideAB) // also left or right of ac ?
        return false;

    if ((c.x() - b.x()) * (p.y() - b.y()) - (c.y() - b.y()) * (p.x() - b.x()) > 0 != sideAB)
        return false;

    return true;
}

/**
 * Ear-Clipping-Algorithm
 */
std::vector<Vec2f> triangulatePolygon(const std::vector<Vec2f>& points) {
    std::vector<Vec2f> triangles{};
    const bool clockwise = isPolygonClockwise(points);

    std::vector<int> indices{};
    for (int i = 0; i < points.size(); ++i) {
        indices.emplace_back(i);
    }

    while (indices.size() > 3) {
        bool earFound = false;
        for (int i = 0; i < indices.size(); ++i) {
            int a = indices[i];
            int b = indices[(i + indices.size() - 1) % indices.size()];
            int c = indices[(i + 1) % indices.size()];

            const Vec2f& va = points[a];
            const Vec2f& vb = points[b];
            const Vec2f& vc = points[c];

            Vec2f vab = vb - va;
            Vec2f vac = vc - va;

            if ((vab.x() * vac.y() - vac.x() * vab.y() < 0.f) != clockwise) // is not ear
                continue;

            bool intersection = false;
            for (int j = 0; j < points.size(); ++j) {
                if (j == a || j == b || j == c) continue;

                if (isPointInTriangle(points[j], va, vb, vc)) {
                    intersection = true;
                    break;
                }
            }
            if (intersection) continue;

            earFound = true;

            triangles.emplace_back(va);
            triangles.emplace_back(vb);
            triangles.emplace_back(vc);

            indices.erase(indices.begin() + i);

            if (indices.size() <= 3) break;
        }

        if (!earFound)
            return triangles;
    }

    triangles.emplace_back(points[indices[0]]);
    triangles.emplace_back(points[indices[1]]);
    triangles.emplace_back(points[indices[2]]);

    return triangles;
}
