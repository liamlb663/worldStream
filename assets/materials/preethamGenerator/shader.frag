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

vec3 getDirection(uint faceIndex, vec2 uv) {
    uv = uv * 2.0 - 1.0;

    switch (faceIndex) {
        case 0: return normalize(vec3( 1.0,  -uv.y,  -uv.x));
        case 1: return normalize(vec3(-1.0, -uv.y, uv.x));
        case 2: return normalize(vec3( uv.x,  1.0,    uv.y));
        case 3: return normalize(vec3( uv.x, -1.0,   -uv.y));
        case 4: return normalize(vec3( uv.x, -uv.y,  1.0));
        case 5: return normalize(vec3(-uv.x, -uv.y, -1.0));
        default: return vec3(0.0);
    }
}

vec3 computePreethamSkyColor(vec3 viewDir, vec3 sunDir, float turbidity) {
    viewDir = normalize(viewDir);
    sunDir = normalize(sunDir);

    float theta = acos(clamp(viewDir.z, -1.0, 1.0));
    float gamma = acos(clamp(dot(viewDir, sunDir), -1.0, 1.0));
    float thetaS = acos(clamp(sunDir.z, 0.0, 1.0));
    thetaS = clamp(thetaS, 0.0, radians(89.9));

    float T = turbidity;

    // Perez coefficients
    float A =  0.1787 * T - 1.4630;
    float B = -0.3554 * T + 0.4275;
    float C = -0.0227 * T + 5.3251;
    float D =  0.1206 * T - 2.5771;
    float E = -0.0670 * T + 0.3703;

    float cosTheta = max(0.01, cos(theta));
    float perez = (1.0 + A * exp(B / cosTheta)) *
                  (1.0 + C * exp(D * gamma) + E * cos(gamma) * cos(gamma));

    float zenithPerez = (1.0 + A * exp(B)) *
                        (1.0 + C * exp(D * thetaS) + E * cos(thetaS) * cos(thetaS));

    // Zenith chromaticity from Preetham
    vec3 zenithRGB;
    zenithRGB = vec3(
        0.65 + 0.15 * (1.0 - sunDir.z),
        0.75,
        1.0
    );

    // Final sky color (relative to zenith)
    return zenithRGB * (perez / zenithPerez);
}

void main() {
    vec3 viewDir = getDirection(pushData.layer, uv);
    vec3 sunDir = normalize(pushData.sunDirection);
    vec3 skyColor = computePreethamSkyColor(viewDir, sunDir, pushData.turbidity);

    skyColor *= pushData.exposure;
    skyColor = pow(skyColor, vec3(1.0 / 2.2));              // gamma correction

    outColor = vec4(clamp(skyColor, 0.0, 1.0), 1.0);
}
