#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragUV;

layout(set = 0, binding = 0) uniform UBO {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(set = 0, binding = 1) uniform OffsetUBO {
    vec3 positionOffset;
    float padding;
} offsetUBO;

layout(push_constant) uniform PushConstants {
    vec4 data0;  // you can repurpose this 128-byte space as needed
    vec4 data1;
    vec4 data2;
    vec4 data3;
    vec4 data4;
    vec4 data5;
    vec4 data6;
    vec4 data7;
} pc;

void main() {
    fragNormal = mat3(transpose(inverse(ubo.model))) * inNormal;
    fragUV = inUV;

    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
}

