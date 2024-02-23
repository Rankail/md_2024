#version 450 core

in vec2 vCoord;
out vec4 FragColor;

uniform sampler2D uTex;

#include "rsc/shader/common/brush.frag"

void main() {
    if (uBrushType == BRUSH_TYPE_NONE) {
        FragColor = texture(uTex, vCoord);
        return;
    }

    FragColor = texture(uTex, vCoord) * brush();
}
