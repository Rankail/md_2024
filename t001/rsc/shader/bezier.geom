#version 450 core

layout (lines_adjacency) in;
layout (triangle_strip, max_vertices=100) out;

uniform mat4 uProjection;
uniform int uLineWidth;

out vec2 vCoord;

vec2 bezierPos(vec2 p0, vec2 p1, vec2 p2, vec2 p3, float t) {
    return pow(1-t, 3) * p0 + 3 * pow(1-t, 2) * t * p1 + 3 * (1-t) * pow(t, 2) * p2 + pow(t, 3) * p3;
}

vec2 bezierTangent(vec2 p0, vec2 p1, vec2 p2, vec2 p3, float t) {
    return -3 * pow(1-t, 2) * p0 + 3 * pow(1-t, 2) * p1 - 6 * t * (1-t) * p1 - 3 * pow(t, 2) * p2 + 6 * t * (1-t) * p2 + 3 * t*t * p3;
}

vec2 rotate90(vec2 dir) {
    return vec2(-dir.y, dir.x);
}

vec2 transform(vec2 v) {
    return (vec4(v, 0.0, 1.0) * uProjection).xy;
}

void main() {
    vec2 p0 = gl_in[0].gl_Position.xy;
    vec2 p1 = gl_in[1].gl_Position.xy;
    vec2 p2 = gl_in[2].gl_Position.xy;
    vec2 p3 = gl_in[3].gl_Position.xy;

    vec2 tl = min(min(transform(p0), transform(p1)), min(transform(p2), transform(p3)));
    vec2 br = max(max(transform(p0), transform(p1)), max(transform(p2), transform(p3)));

    vec2 boundingPos = vec2(tl.x, br.y);
    vec2 boundingSize = vec2(br.x, tl.y) - boundingPos;

    for (int i = 0; i < 50; ++i) {
        float t = float(i) / 49;

        vec2 pos = bezierPos(p0, p1, p2, p3, t);
        vec2 tangent = normalize(bezierTangent(p0, p1, p2, p3, t));
        vec2 normal = normalize(rotate90(tangent)) * uLineWidth / 2.f;

        vec2 pos0 = transform(pos - normal);
        vec2 pos1 = transform(pos + normal);

        vCoord = (pos0 - boundingPos) / boundingSize;
        gl_Position = vec4(pos0, 0.0, 1.0);
        EmitVertex();

        vCoord = (pos1 - boundingPos) / boundingSize;
        gl_Position = vec4(pos1, 0.0, 1.0);
        EmitVertex();
    }

    EndPrimitive();
}