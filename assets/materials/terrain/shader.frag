#version 450

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragUV;
layout(location = 3) in vec3 fragTangent;
layout(location = 4) in vec3 fragBitangent;

layout(location = 0) out vec4 outColor;

// Global Data
layout(set = 0, binding = 0) uniform GlobalUBO {
    mat4 view;
    mat4 proj;
    vec3 cameraPosition;
    float time;
};

layout(set = 0, binding = 1) uniform LightUBO {
    vec3 directionalLightDir;
    float _pad2;
    vec3 directionalLightColor;
    float intensity;
};

// Textures
layout(set = 1, binding = 0) uniform sampler2D heightMap;

// Object
layout(set = 2, binding = 0) uniform ObjectUBO {
    mat4 model;
};

void main() {
    float height = texture(heightMap, fragUV).r;
    outColor = vec4(vec3(height), 1.0);
}

