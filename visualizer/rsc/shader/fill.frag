#version 450 core

in vec2 vCoord;
out vec4 FragColor;

#include "rsc/shader/common/brush.frag"

void main() {
    FragColor = brush();
}
