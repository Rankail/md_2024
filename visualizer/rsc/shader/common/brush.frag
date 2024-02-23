#define BRUSH_TYPE_SOLID      0x00
#define BRUSH_TYPE_LINEAR     0x01
#define BRUSH_TYPE_LINEAR2    0x04
#define BRUSH_TYPE_RADIAL     0x02
#define BRUSH_TYPE_ELLIPTICAL 0x03
#define BRUSH_TYPE_NONE       0xFF

uniform int uBrushType;
uniform vec2 uStartPos;
uniform vec2 uEndPos;
uniform vec2 uAuxPos;
uniform float uBias;
uniform vec4 uStartColor;
uniform vec4 uEndColor;

float brush_getBias(float bias, float t) {
    return (t / ((( (1.0/bias) - 2.0 ) * (1.0 - t)) + 1.0));
}

vec4 brush_createSolidBrush() {
    return uStartColor;
}

vec4 brush_createLinearBrush() {
    vec2 uv = vCoord;

    vec2 dir = uEndPos - uStartPos;
    vec2 pos = uv - uStartPos;
    float t = dot(dir, pos) / dot(dir, dir);

    float bias = brush_getBias(uBias, t);

    vec4 color = mix(uStartColor, uEndColor, bias);

    return color;
}

vec4 brush_createLinear2Brush() {
    vec2 uv = vCoord;

    vec2 dir = uEndPos - uStartPos;
    vec2 pos = uv - uStartPos;
    float t = dot(dir, pos) / dot(dir, dir);

    float bias = brush_getBias(uBias, t);

    vec4 color = mix(uStartColor, uEndColor, bias);

    return color;
}

vec4 brush_createEllipicialBrush(vec2 auxPos) {
    vec2 uv = vCoord;

    vec2 dir = uEndPos - uStartPos;
    vec2 pos = uv - uStartPos;
    float t = dot(dir, pos) / dot(dir, dir);

    float bias = brush_getBias(uBias, t);

    vec4 color = mix(uStartColor, uEndColor, bias);

    return color;
}

vec4 brush_createMissingNo() {
    int pos = int(floor(vCoord.x * 16) + floor(vCoord.y * 16));

    if (pos % 2 == 0) {
        return vec4(1,0,1,1);
    } else {
        return vec4(0,0,0,1);
    }
}

vec4 brush() {
    switch (uBrushType) {
        case BRUSH_TYPE_SOLID:      return brush_createSolidBrush();
        case BRUSH_TYPE_LINEAR:     return brush_createLinearBrush();
        case BRUSH_TYPE_LINEAR2:    return brush_createLinear2Brush();
        case BRUSH_TYPE_RADIAL:     return brush_createEllipicialBrush(uEndPos);
        case BRUSH_TYPE_ELLIPTICAL: return brush_createEllipicialBrush(uAuxPos);
    }

    return brush_createMissingNo();
}