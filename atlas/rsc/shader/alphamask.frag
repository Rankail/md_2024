#version 450 core

#define COLOR_WHITE vec3(1,1,1)

in vec2 vCoord;
out vec4 FragColor;

uniform sampler2D uTex;

#include "rsc/shader/common/brush.frag"

// Color-keys black out of the image and
// leaves the rest as a white image
// This is the main difference to the
// texture shader.
vec4 colorkey(vec4 vin) {
    return vec4(COLOR_WHITE, vin.r); // We assume texture is black and white only.
}

void main() {
    if (uBrushType == BRUSH_TYPE_NONE) {
        FragColor = colorkey(texture(uTex, vCoord));
        return;
    }

    FragColor = colorkey(texture(uTex, vCoord)) * brush();
}
