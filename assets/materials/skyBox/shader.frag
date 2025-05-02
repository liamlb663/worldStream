#version 450

layout(set = 0, binding = 0) uniform samplerCube u_Skybox;

layout(location = 0) in vec3 v_Direction;
layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(u_Skybox, normalize(v_Direction));
}

