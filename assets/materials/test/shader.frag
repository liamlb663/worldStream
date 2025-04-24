#version 450

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

// Descriptor Set 0: Lighting
struct PointLight {
    vec3 position;
    float _pad1;
    vec3 color;
    float intensity;
};

layout(set = 0, binding = 1) uniform LightUBO {
    vec3 directionalLightDir;
    float _pad2;
    vec3 directionalLightColor;
    float _pad3;
    PointLight pointLights[4];
    int numPointLights;
};

// Descriptor Set 0: Global Frame Data
layout(set = 0, binding = 0) uniform GlobalUBO {
    mat4 view;
    mat4 proj;
    vec3 cameraPosition;
    float time;
};

// Descriptor Set 1: Material Textures
layout(set = 1, binding = 0) uniform sampler2D albedoMap;
layout(set = 1, binding = 1) uniform sampler2D normalMap;
layout(set = 1, binding = 2) uniform sampler2D metallicRoughnessMap;

// Descriptor Set 2: Object Tint (also used in vertex)
layout(set = 2, binding = 0) uniform ObjectUBO {
    mat4 model;
    vec4 colorTint;
};

// Push Constants
layout(push_constant) uniform PushData {
    vec4 highlightColor;
    float outlineWidth;
};

void main() {
    vec3 N = normalize(fragNormal);
    vec3 V = normalize(cameraPosition - fragPos);

    vec3 albedo = texture(albedoMap, fragUV).rgb;
    vec3 color = vec3(0.0);

    // Directional light
    vec3 Ld = normalize(-directionalLightDir);
    float diffD = max(dot(N, Ld), 0.0);
    color += directionalLightColor * diffD;

    // Point lights
    for (int i = 0; i < numPointLights; ++i) {
        vec3 Lp = normalize(pointLights[i].position - fragPos);
        float dist = length(pointLights[i].position - fragPos);
        float diffP = max(dot(N, Lp), 0.0);
        float atten = pointLights[i].intensity / (dist * dist);
        color += pointLights[i].color * diffP * atten;
    }

    // Apply base albedo and tint
    color *= albedo * colorTint.rgb;

    // Optional push constant tinting
    color = mix(color, highlightColor.rgb, highlightColor.a);

    outColor = vec4(color, 1.0);
}

