#version 450

layout(location = 0) in vec3 a_Position;

layout(push_constant) uniform PushConstants {
    mat4 u_ViewProj;
} pc;

layout(location = 0) out vec3 v_Direction;

void main() {
    v_Direction = a_Position;

    gl_Position = pc.u_ViewProj * vec4(a_Position, 1.0);
    gl_Position.z = gl_Position.w * 0.9999;
}
