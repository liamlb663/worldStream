#version 450

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

// Lighting
layout(set = 0, binding = 1) uniform LightUBO {
    vec3 directionalLightDir;
    float _pad2;
    vec3 directionalLightColor;
    float intensity;
};

layout(set = 0, binding = 0) uniform GlobalUBO {
    mat4 view;
    mat4 proj;
    vec3 cameraPosition;
    float time;
};

// Textures
layout(set = 1, binding = 0) uniform sampler2D albedoMap;
layout(set = 1, binding = 1) uniform sampler2D normalMap;
layout(set = 1, binding = 2) uniform sampler2D metallicRoughnessMap;

// Object
layout(set = 2, binding = 0) uniform ObjectUBO {
    mat4 model;
    vec4 colorTint;
};

// Converts normal from [0,1] → [-1,1]
vec3 decodeNormal(vec3 n) {
    return normalize(n * 2.0 - 1.0);
}

void main() {
    vec3 V = normalize(cameraPosition - fragPos);

    // Sample textures
    vec3 albedo = texture(albedoMap, fragUV).rgb;
    vec3 normalMapVal = texture(normalMap, fragUV).rgb;
    vec2 metalRough = texture(metallicRoughnessMap, fragUV).rg;

    float metallic = metalRough.r;
    float roughness = clamp(metalRough.g, 0.05, 1.0); // avoid div0

    float shininess = mix(8.0, 128.0, 1.0 - roughness); // rough = low shininess
    float specularStrength = mix(0.04, 1.0, metallic);  // non-metals reflect less

    // Use normal map in world space (approx — no TBN used yet)
    vec3 N = normalize(fragNormal);
    vec3 normalTex = decodeNormal(normalMapVal);
    N = normalize(mix(N, normalTex, 1.0)); // crude approximation without TBN

    vec3 color = vec3(0.0);

    // Directional light
    vec3 Ld = normalize(-directionalLightDir);
    vec3 H = normalize(V + Ld);
    float diff = max(dot(N, Ld), 0.0);
    float spec = pow(max(dot(N, H), 0.0), shininess);

    color += directionalLightColor * (diff + specularStrength * spec);

    // Final color tinting
    color *= albedo * colorTint.rgb;
    outColor = vec4(color, 1.0);
}

