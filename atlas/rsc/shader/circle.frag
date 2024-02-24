#version 450 core

in vec2 vCoord;
out vec4 FragColor;

#include "rsc/shader/common/brush.frag"

vec4 circle() {
    vec2 relPos = vCoord - vec2(0.5, 0.5);
    float dist = relPos.x * relPos.x + relPos.y * relPos.y;
    return (dist <= 0.25) ? vec4(1.0, 1.0, 1.0, 1.0) : vec4(1.0, 1.0, 1.0, 0.0);
}

void main() {
    FragColor = vec4(1.0, 1.0, 0.0, 1.0);
    if (uBrushType == BRUSH_TYPE_NONE) {
        FragColor = circle();
        return;
    }

    FragColor = circle() * brush();
}