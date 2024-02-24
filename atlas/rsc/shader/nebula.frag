#version 450 core

in vec2 vCoord;
out vec4 FragColor;

uniform int uTime;
uniform sampler2D uTex;

#include "rsc/shader/common/brush.frag"
#include "rsc/shader/common/noise.frag"

vec4 nebula() {
    vec2 st = gl_FragCoord.xy / 70.0;
    // st += st * abs(sin(u_time*0.1)*3.0);
    vec3 color = vec3(0.0);

    float time = uTime / 600.0f;

    vec2 q = vec2(0.);
    q.x = fbm( st * 0.5 );
    q.y = fbm( st );

    vec2 r = vec2(0.);
    r.x = fbm( st + 1.0*q + vec2(1.7,9.2)+ 0.15* time);
    r.y = fbm( st + 1.0*q + vec2(8.3,2.8)+ 0.126*time);

    float f = fbm(st+r);

    color = mix(vec3(0.5,0.5,0.5),
                vec3(0.5,0.5,0.5),
                clamp(f,1.0,1.0));

    color = mix(color,
                vec3(0.5,0.5,0.5),
                clamp(length(q),1.0,0.0));

    color = mix(color,
                vec3(1,1,1),
                clamp(length(r.x),1.0,0.0));

    return vec4( (f*f*f+.6*f*f+.5*f) * color, 1.);
}

void main() {
    if (uBrushType == BRUSH_TYPE_NONE) {
        FragColor = texture(uTex, vCoord) * nebula();
        return;
    }

    FragColor = texture(uTex, vCoord) * brush() * nebula();
}
