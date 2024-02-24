#version 450 core

layout (lines) in;
layout (triangle_strip, max_vertices=4) out;

uniform mat4 uProjection;
uniform int uLineWidth;

out vec2 vCoord;

vec2 rotate90(vec2 dir) {
    return vec2(-dir.y, dir.x);
}

vec2 transform(vec2 v) {
    return (vec4(v, 0.0, 1.0) * uProjection).xy;
}

void main() {
    vec2 p0 = gl_in[0].gl_Position.xy;
    vec2 p1 = gl_in[1].gl_Position.xy;

    vec2 tl = min(transform(p0), transform(p1));
    vec2 br = max(transform(p0), transform(p1));

    vec2 boundingPos = vec2(tl.x, br.y);
    vec2 boundingSize = vec2(br.x, tl.y) - boundingPos;

    vec2 normal = normalize(rotate90(p1 - p0)) * uLineWidth / 2.f;

    vec2 pos0 = transform(p0 - normal);
    vCoord = vec2(0.f, 0.f);
    gl_Position = vec4(pos0, 0.0, 1.0);
    EmitVertex();

    vec2 pos1 = transform(p0 + normal);
    vCoord = vec2(0.f, 1.f);
    gl_Position = vec4(pos1, 0.0, 1.0);
    EmitVertex();

    vec2 pos2 = transform(p1 - normal);
    vCoord = vec2(1.f, 0.f);
    gl_Position = vec4(pos2, 0.0, 1.0);
    EmitVertex();

    vec2 pos3 = transform(p1 + normal);
    vCoord = vec2(1.f, 1.f);
    gl_Position = vec4(pos3, 0.0, 1.0);
    EmitVertex();

    EndPrimitive();
}