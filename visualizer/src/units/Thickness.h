#ifndef MD_2024_THICKNESS_H
#define MD_2024_THICKNESS_H

#include <utils/utils.h>

/**
 * Represents a thickness of a margin, padding
 * border or other relative object.
 */
struct Thickness {
    unsigned l;
    unsigned t;
    unsigned r;
    unsigned b;

    Thickness() : l(0), t(0), r(0), b(0) {}

    Thickness(unsigned l, unsigned t, unsigned r, unsigned b) {
        this->l = l;
        this->t = t;
        this->r = r;
        this->b = b;
    }

    Thickness(unsigned lr, unsigned tb) {
        l = r = lr;
        t = b = tb;
    }

    Thickness(unsigned a) {
        l = r = t = b = a;
    }
};

#endif
