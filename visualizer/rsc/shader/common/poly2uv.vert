#version 450 core

layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aUV;

out vec2 vCoord;

uniform mat4 uProjection;

void main() {
    gl_Position = vec4(aPosition, 0.0, 1.0) * uProjection;
    vCoord = aUV;
}