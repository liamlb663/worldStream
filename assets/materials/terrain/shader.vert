#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragUV;

// Descriptor Set 0: Global Frame Data
layout(set = 0, binding = 0) uniform GlobalUBO {
    mat4 view;
    mat4 proj;
    vec3 cameraPosition;
    float time;
};

layout(set = 1, binding = 0) uniform sampler2D normalHeightMap;

// Tweakable parameters
const float heightScale = 0.25;
const mat4 model = mat4(1.0f);

void main() {
    // Sample normal and height from texture
    vec4 packed = texture(normalHeightMap, inUV);
    vec3 bakedNormal = normalize(packed.rgb * 2.0 - 1.0); // unpack normal
    float height = packed.a;

    // Displace vertex position along input normal (or bakedNormal if preferred)
    float offset = heightScale * (height - 0.5f);
    vec3 displacedPosition = inPosition + inNormal * offset;

    // Output
    vec4 worldPos = model * vec4(displacedPosition, 1.0);
    fragPos = worldPos.xyz;
    fragNormal = mat3(model) * bakedNormal;
    fragUV = inUV;

    gl_Position = proj * view * worldPos;
}

