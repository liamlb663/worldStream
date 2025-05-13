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

// Tweakable parameters
const float heightScale = 0.25;
const float uvOffset = 0.001;

void main() {
    // Base height
    float baseHeight = texture(heightMap, inUV).r;
    float offset = heightScale * (baseHeight - 0.5f);

    // Displace vertex position
    vec3 displacedPosition = inPosition + inNormal * offset;

    // Estimate partial derivatives of heightmap using UV offsets
    float height_dx = texture(heightMap, inUV + vec2(uvOffset, 0.0)).r;
    float height_dy = texture(heightMap, inUV + vec2(0.0, uvOffset)).r;

    // Approximate displaced tangents
    vec3 dPos_dx = tangent + inNormal * heightScale * (height_dx - baseHeight) / uvOffset;
    vec3 dPos_dy = bitangent + inNormal * heightScale * (height_dy - baseHeight) / uvOffset;

    // Recalculate normal using cross product of displaced tangents
    vec3 displacedNormal = normalize(cross(dPos_dx, dPos_dy));
    vec3 displacedTangent = normalize(dPos_dx);
    vec3 displacedBitangent = normalize(cross(displacedNormal, displacedTangent)); // ensure orthogonality

    // Output
    vec4 worldPos = model * vec4(displacedPosition, 1.0);
    fragPos = worldPos.xyz;
    fragNormal = mat3(model) * displacedNormal;
    fragUV = inUV;
    fragTangent = mat3(model) * displacedTangent;
    frgaBitangent = mat3(model) * displacedBitangent;

    gl_Position = proj * view * worldPos;
}

