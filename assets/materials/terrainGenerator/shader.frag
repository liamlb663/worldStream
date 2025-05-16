#version 450

layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 outColor;

layout(push_constant) uniform PushConstants {
    float scale;
    float seed;
    vec2 chunkOffset;
    float texelSize;
    int octaves;
} pc;

#include "noiseFunctions.glsl"

// Now with Fractal Brownian Motion!

float fbm(vec2 pos, float scale, float seed, vec2 offset) {
    float total = 0.0;
    float amplitude = 1.0;
    float frequency = 1.0;
    float persistence = 0.5; // controls how quickly amplitude drops
    //int octaves = 8;

    for (int i = 0; i < pc.octaves; ++i) {
        total += amplitude * perlin(pos * frequency, scale * frequency, seed + float(i) * 237.0, offset);
        amplitude *= persistence;
        frequency *= 2.0;
    }

    return total;
}

vec3 computeFbmNormal(vec2 p, float eps, float scale, float seed, vec2 offset) {
    float h = fbm(p, scale, seed, offset);
    float hx = fbm(p + vec2(eps, 0.0), scale, seed, offset);
    float hy = fbm(p + vec2(0.0, eps), scale, seed, offset);

    vec3 dx = vec3(eps, 0.0, hx - h);
    vec3 dy = vec3(0.0, eps, hy - h);

    return normalize(cross(dx, dy)); // Z-up normal
}

void main() {
    vec2 scaledUV = uv * pc.scale * (1.0 + pc.texelSize);
    float height = fbm(scaledUV, pc.scale, pc.seed, pc.chunkOffset);
    vec3 normal = computeFbmNormal(scaledUV, 0.001, pc.scale, pc.seed, pc.chunkOffset);

    // Pack normal from [-1,1] to [0,1]
    vec3 packedNormal = normal * 0.5 + 0.5;

    outColor = vec4(packedNormal, height);
}

