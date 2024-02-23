#ifndef MD_2024_RECT_H
#define MD_2024_RECT_H

#include <utils/utils.h>
#include "Point.h"
#include "Size.h"

struct Rect {
    int x;
    int y;
    unsigned dx;
    unsigned dy;

    Rect(Point pt, int dx, int dy)
        : x(pt.x), y(pt.y), dx(dx), dy(dy) {}

    Rect(Point pt, Size sz)
        : x(pt.x), y(pt.y), dx(sz.width), dy(sz.height) {}

    Rect(int x, int y, unsigned dx, unsigned dy)
        : x(x), y(y), dx(dx), dy(dy) {}

    bool contains(Point pt) const {
        return (pt.x >= x && pt.y >= y)
            && (pt.x <= x + dx && pt.y <= y + dy);
    }
};

#endif