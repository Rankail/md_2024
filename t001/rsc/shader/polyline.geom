#version 450 core

layout (lines) in;
layout (triangle_strip, max_vertices=4) out;

in VS_OUT {
    vec2 corner;
} gs_in[];

uniform mat4 uProjection;
uniform int uLineWidth;
uniform vec2 uTopLeft;
uniform vec2 uBoundingSize;

out vec2 vCoord;

vec2 transform(vec2 v) {
    return (vec4(v, 0.0, 1.0) * uProjection).xy;
}

vec2 clamp01(vec2 v) {
    return max(vec2(0.0f, 0.0f), min(v, vec2(1.0f, 1.0f)));
}

void addVertex(vec2 v) {
    vCoord = clamp01((v - uTopLeft) / uBoundingSize);
    gl_Position = vec4(transform(v), 0.0, 1.0);
    EmitVertex();
}

void main() {
    vec2 p0 = gl_in[0].gl_Position.xy;
    vec2 p1 = gl_in[1].gl_Position.xy;

    vec2 normal0 = gs_in[0].corner * uLineWidth / 2.f;
    vec2 normal1 = gs_in[1].corner * uLineWidth / 2.f;

    addVertex(p0 - normal0);
    addVertex(p0 + normal0);
    addVertex(p1 - normal1);
    addVertex(p1 + normal1);

    EndPrimitive();
}