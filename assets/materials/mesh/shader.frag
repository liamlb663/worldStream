#version 450

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 2) uniform sampler2D colorTexture;

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
    outColor = vec4(fragUV, 0.0, 1.0);
    vec4 texColor = texture(colorTexture, fragUV);
    outColor = texColor;
}

