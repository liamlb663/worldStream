#version 450

layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 outColor;

layout(push_constant) uniform PushConstants {
    float scale;
    float seed;
    vec2 chunkOffset;
    float texelSize;
} pc;

#include "noiseFunctions.glsl"

void main() {
    vec2 scaledUV = uv * pc.scale * (1.0 + pc.texelSize);
    float height = perlin(scaledUV, pc.scale, pc.seed, pc.chunkOffset);
    vec3 normal = computeNormal(scaledUV, 0.001, pc.scale, pc.seed, pc.chunkOffset);

    // Pack normal from [-1,1] to [0,1]
    vec3 packedNormal = normal * 0.5 + 0.5;

    outColor = vec4(packedNormal, height);
}

