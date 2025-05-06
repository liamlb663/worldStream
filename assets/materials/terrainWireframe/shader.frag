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

// Converts normal from [0,1] → [-1,1]
vec3 decodeNormal(vec3 n) {
    return normalize(n * 2.0 - 1.0);
}

void main() {
    // Constants
    const vec3 baseAlbedo = vec3(1.0, 0.8, 0.6);
    const float metallic = 0.0;
    const float roughness = 0.5;
    const float MIN_ROUGHNESS = 0.05;
    const float MIN_SHININESS = 8.0;
    const float MAX_SHININESS = 128.0;
    const float MIN_SPECULAR = 0.04;

    vec3 V = normalize(cameraPosition - fragPos);

    float clampedRoughness = clamp(roughness, MIN_ROUGHNESS, 1.0);
    float shininess = mix(MIN_SHININESS, MAX_SHININESS, 1.0 - clampedRoughness);
    float specularStrength = mix(MIN_SPECULAR, 1.0, metallic);

    // Build TBN matrix from inputs
    vec3 T = normalize(fragTangent);
    vec3 B = normalize(fragBitangent);
    vec3 N = normalize(fragNormal);
    mat3 TBN = mat3(T, B, N);

    float height = texture(heightMap, fragUV).r;
    N = normalize(N + vec3(0.0, 0.0, height * 0.05));

    // Lighting
    vec3 Ld = normalize(-directionalLightDir);
    vec3 H = normalize(V + Ld);
    float diff = max(dot(N, Ld), 0.0);
    float spec = pow(max(dot(N, H), 0.0), shininess);

    vec3 lightColor = directionalLightColor * intensity;
    vec3 color = lightColor * (diff + specularStrength * spec);

    // Final color
    color *= baseAlbedo;
    outColor = vec4(color, 1.0);
}

