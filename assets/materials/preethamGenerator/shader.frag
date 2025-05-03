#version 450

layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 outColor;

layout(push_constant) uniform Push {
    uint layer;
    float _pad0;
    vec3 sunDirection;
    float turbidity;
    float exposure;
    float _pad1;
} pushData;

// Remap Y-up direction to Z-up by rotating -90° around X
vec3 toZUp(vec3 dir) {
    return vec3(dir.x, -dir.z, dir.y);
}

// View direction: remap from UV + layer to a cubemap direction
vec3 getDirection(uint faceIndex, vec2 uv) {
    uv = uv * 2.0 - 1.0;
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

vec3 computePreethamSkyColor(vec3 viewDir, vec3 sunDir, float turbidity) {
    // Zenith angle (view direction elevation)
    float theta = acos(clamp(viewDir.z, -1.0, 1.0)); // angle from up
    float gamma = acos(clamp(dot(viewDir, sunDir), -1.0, 1.0)); // angle between view & sun

    // Empirical coefficients from Preetham model
    float T = turbidity;
    float A =  0.1787 * T - 1.4630;
    float B = -0.3554 * T + 0.4275;
    float C = -0.0227 * T + 5.3251;
    float D =  0.1206 * T - 2.5771;
    float E = -0.0670 * T + 0.3703;

    // Perez function
    float perezTheta = (1.0 + A * exp(B / cos(theta)));
    float perezGamma = (1.0 + C * exp(D * gamma) + E * cos(gamma) * cos(gamma));
    float luminance = perezTheta * perezGamma;

    // Base zenith color approximation (RGB)
    vec3 zenithColor = vec3(0.75, 0.85, 1.0); // Slightly pale blue
    return zenithColor * luminance;
}

void main() {
    vec3 viewDir = toZUp(getDirection(pushData.layer, uv));
    vec3 skyColor = computePreethamSkyColor(viewDir, normalize(pushData.sunDirection), pushData.turbidity);
    skyColor *= pushData.exposure;

    outColor = vec4(skyColor, 1.0);
}

