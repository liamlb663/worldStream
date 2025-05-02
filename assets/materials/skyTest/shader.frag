#version 450

layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 outColor;

layout(push_constant) uniform Push {
    float time;
    uint layer;
} pushData;

void main() {
    float t = 0.5 + 0.5 * sin(pushData.time*10.0f + uv.x * 10.0);
    vec3 color = mix(vec3(0.1, 0.2, 0.5), vec3(0.8, 0.4, 0.2), uv.y + 0.1 * t);
    color += float(pushData.layer) * 0.05; // Slight brightness based on layer
    outColor = vec4(color, 1.0);
}

