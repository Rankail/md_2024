#ifndef MD_2024_BRUSH_H
#define MD_2024_BRUSH_H

#include <utility>
#include <vector>

#include "Color.h"

enum class BrushType : int {
    SOLID  = 0x00,
    LINEAR = 0x01,
    RADIAL = 0x02,
    ELLIPTICAL = 0x03,
    LINEAR2 = 0x04
};

struct Brush {
    BrushType type;

    Vec2f positionStart;
    Vec2f positionEnd;
    Vec2f positionAux;

    Color colorStart;
    Color colorEnd;

    float bias;

    static Brush linear(Color cStart, Color cEnd, const Vec2f& start = {0, 0.5}, const Vec2f& end = {1, 0.5}, float bias = 0.5) {
        return {BrushType::LINEAR, start, end, {0,0}, cStart, cEnd, bias};
    }

    static Brush linear2(Color cStart, Color cEnd, const Vec2f& start = {0, 0.5}, const Vec2f& end = {1, 0.5}, float bias = 0.5) {
        return {BrushType::LINEAR2, start, end, {0,0}, cStart, cEnd, bias};
    }

    static Brush elliptical(Color cStart, Color cEnd, const Vec2f& start = {0.5, 0.5}, const Vec2f& end = {1, 0.5}, const Vec2f& aux = {0.5, 1}, float bias = 0.5) {
        return {BrushType::ELLIPTICAL, start, end, aux, cStart, cEnd, bias};
    }

    static Brush elliptical(Color cStart, Color cEnd, const Vec2f& start = {0.5, 0.5}, const Vec2f& end = {1, 0.5}, float bias = 0.5) {
        return {BrushType::RADIAL, start, end, {0,0}, cStart, cEnd, bias};
    }

    static Brush solid(Color color) {
        return {BrushType::SOLID, {0,0}, {0,0}, {0,0}, color, {0,0,0}, 0};
    }
};

#endif
