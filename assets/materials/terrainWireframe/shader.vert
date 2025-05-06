#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragUV;
layout(location = 3) out vec3 fragTangent;
layout(location = 4) out vec3 frgaBitangent;

// Descriptor Set 0: Global Frame Data
layout(set = 0, binding = 0) uniform GlobalUBO {
    mat4 view;
    mat4 proj;
    vec3 cameraPosition;
    float time;
};

layout(set = 1, binding = 0) uniform sampler2D heightMap;

// Descriptor Set 2: Object-specific data
layout(set = 2, binding = 0) uniform ObjectUBO {
    mat4 model;
};

void main() {
    vec4 worldPos = model * vec4(inPosition, 1.0);
    fragPos = worldPos.xyz;
    fragNormal = mat3(model) * inNormal;
    fragUV = inUV;

    float offset = texture(heightMap, inUV).r;
    worldPos.z += offset;

    gl_Position = proj * view * worldPos;
}
