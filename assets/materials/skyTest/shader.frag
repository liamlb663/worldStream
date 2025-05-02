#version 450

layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 outColor;

layout(push_constant) uniform Push {
    float time;
    uint layer;
} pushData;

vec3 getDirection(uint faceIndex, vec2 uv) {
    uv = uv * 2.0 - 1.0; // Remap from [0,1] to [-1,1]

    switch (faceIndex) {
        case 0: return normalize(vec3( 1.0,  -uv.x,   -uv.y));  // +X
        case 1: return normalize(vec3(-1.0,   uv.x,   -uv.y));  // -X
        case 2: return normalize(vec3( uv.x,  uv.y,    1.0));   // +Z (up)
        case 3: return normalize(vec3( uv.x, -uv.y,   -1.0));   // -Z (down)
        case 4: return normalize(vec3( uv.x,  1.0,   -uv.y));   // +Y
        case 5: return normalize(vec3(-uv.x, -1.0,   -uv.y));   // -Y
        default: return vec3(0.0);
    }
}

void main() {
    vec3 dir = getDirection(pushData.layer, uv);
    float y = dir.y; // Vertical component

    // Clamp for blending
    float up = clamp((y - 0.0) / 0.6, 0.0, 1.0);    // Horizon to Sky
    float down = clamp((-y - 0.0) / 0.5, 0.0, 1.0); // Horizon to Ground

    // Colors
    vec3 skyColor = vec3(0.2, 0.5, 1.0);   // Deep sky blue
    vec3 horizonColor = vec3(0.9, 0.9, 0.8); // Pale horizon
    vec3 groundColor = vec3(0.8, 0.4, 0.2);  // Warm orange

    // Interpolate: ground ← horizon → sky
    vec3 color = mix(horizonColor, skyColor, up);
    color = mix(groundColor, color, 1.0 - down);

    outColor = vec4(color, 1.0);
}

