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

// Descriptor Set 1: Terrain Data
layout(set = 1, binding = 0) uniform TerrainUBO {
    float terrainScale;
    float heightScale;
};

// Descriptor Set 2 / Push Constants: Chunk Data
layout(set = 2, binding = 0) uniform sampler2D normalHeightMap;
layout(push_constant) uniform PushConstants {
    vec2 chunkOffset;
} pc;

mat4 scale(mat4 m, vec3 s) {
    mat4 scaleMatrix = mat4(
        vec4(s.x, 0.0, 0.0, 0.0),
        vec4(0.0, s.y, 0.0, 0.0),
        vec4(0.0, 0.0, s.z, 0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );
    return m * scaleMatrix;
}

void main() {
    // Sample normal and height from texture
    vec4 packed = texture(normalHeightMap, inUV);
    vec3 bakedNormal = normalize(packed.rgb * 2.0 - 1.0); // unpack normal
    float height = packed.a;

    // Displace vertex position along input normal (or bakedNormal if preferred)
    float verticalScale = heightScale * terrainScale;
    float offset = verticalScale * (height - 0.5f);
    vec3 displacedPosition = (inPosition + vec3(pc.chunkOffset, 0.0f)) + inNormal * offset;

    mat4 model = scale(mat4(1.0), vec3(terrainScale, terrainScale, 1.0));

    // Output
    vec4 worldPos = model * vec4(displacedPosition, 1.0);
    fragPos = worldPos.xyz;
    fragUV = inUV;

    mat3 normalMatrix = mat3(
        1.0 / terrainScale, 0.0, 0.0,
        0.0, 1.0 / terrainScale, 0.0,
        0.0, 0.0, 1.0 / verticalScale
    );

    // Transform baked normal by normalMatrix and normalize
    fragNormal = normalize(normalMatrix * bakedNormal);

    gl_Position = proj * view * worldPos;
}

