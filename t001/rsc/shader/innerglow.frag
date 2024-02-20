#version 450 core

in vec2 vCoord;
out vec4 FragColor;

uniform uvec2 uSize;
uniform uint uRadius;

#include "rsc/shader/common/brush.frag"

float map(float value, float min1, float max1, float min2, float max2) {
    if (value < min1) return min2;
    if (value > max1) return max2;
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

float dist(vec2 x, vec2 p1, vec2 p2, vec2 p3, vec2 p4) {

    if (x.x < p1.x && x.y > p1.y && x.y < p4.y) {
        return length(x - vec2(p1.x, x.y));
    }

    if (x.x > p2.x && x.y > p2.y && x.y < p3.y) {
        return length(x - vec2(p2.x, x.y));
    }

    if (x.x > p1.x && x.x < p2.x && x.y < p1.y) {
        return length(x - vec2(x.x, p1.y));
    }

    if (x.x > p1.x && x.x < p2.x && x.y > p3.y) {
        return length(x - vec2(x.x, p3.y));
    }

    if (x.x < p1.x && x.y < p1.y) {
        return length(x - p1);
    }

    if (x.x < p4.x && x.y > p4.y) {
        return length(x - p4);
    }

    if (x.x > p2.x && x.y < p2.y) {
        return length(x - p2);
    }

    if (x.x > p3.x && x.y > p3.y) {
        return length(x - p3);
    }

    return 0;
}

vec4 makeGlowMask() {
    vec2 pos = vCoord * uSize;

    float w = uSize.x;
    float h = uSize.y;

    float rorw = min(uSize.x / 2, uRadius);
    float rorh = min(uSize.y / 2, uRadius);

    float d = dist(pos,
                   vec2(rorw, rorh),
                   vec2(w - rorw, rorh),
                   vec2(w - rorw, h - rorh),
                   vec2(rorw, h - rorh));


    float t = map(d, 0.0, uRadius, 0.0, 1.0);

    return vec4(1,1,1,t);
}

void main() {
    FragColor = makeGlowMask() * brush();
}
