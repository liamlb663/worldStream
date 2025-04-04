#version 450

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform UBO {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(push_constant) uniform PushConstants {
    vec4 data0;
    vec4 data1;
    vec4 data2;
    vec4 data3;
    vec4 data4;
    vec4 data5;
    vec4 data6;
    vec4 data7;
} pc;

void main() {
    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    float lighting = max(dot(normal, lightDir), 0.0);

    vec3 baseColor = vec3(fragUV, 1.0);  // UV as color demo
    outColor = vec4(baseColor * lighting, 1.0);
}

