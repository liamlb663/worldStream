#version 450

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform PushConstants {
    float scale;
    float seed;
    vec2 chunkOffset;
} pc;

float grad(int hash, vec2 p) {
    switch (hash & 3) {
        case 0: return  p.x + p.y;
        case 1: return -p.x + p.y;
        case 2: return  p.x - p.y;
        case 3: return -p.x - p.y;
        default: return 0.0;
    }
}

float hash2D(vec2 p, float seed) {
    return fract(sin(dot(p + seed, vec2(127.1, 311.7))) * 43758.5453123);
}

vec2 randomGradient(vec2 p, float seed) {
    float angle = hash2D(p, seed) * 6.2831853; // 2π
    return vec2(cos(angle), sin(angle));
}

vec2 fade(vec2 t) {
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

float perlin(vec2 p) {
    vec2 worldP = p + (pc.chunkOffset * pc.scale);

    vec2 Pi = floor(worldP);
    vec2 Pf = fract(worldP);
    vec2 f = fade(Pf);

    vec2 g00 = randomGradient(Pi + vec2(0.0, 0.0), pc.seed);
    vec2 g10 = randomGradient(Pi + vec2(1.0, 0.0), pc.seed);
    vec2 g01 = randomGradient(Pi + vec2(0.0, 1.0), pc.seed);
    vec2 g11 = randomGradient(Pi + vec2(1.0, 1.0), pc.seed);

    float v00 = dot(g00, Pf - vec2(0.0, 0.0));
    float v10 = dot(g10, Pf - vec2(1.0, 0.0));
    float v01 = dot(g01, Pf - vec2(0.0, 1.0));
    float v11 = dot(g11, Pf - vec2(1.0, 1.0));

    float i1 = mix(v00, v10, f.x);
    float i2 = mix(v01, v11, f.x);
    return 0.5 + 0.5 * mix(i1, i2, f.y);
}

vec3 computeNormal(vec2 p, float eps) {
    float h = perlin(p);
    float hx = perlin(p + vec2(eps, 0.0));
    float hy = perlin(p + vec2(0.0, eps));

    vec3 dx = vec3(eps, 0.0, hx - h);
    vec3 dy = vec3(0.0, eps, hy - h);

    return normalize(cross(dx, dy)); // Z is up
}

void main() {
    vec2 scaledUV = uv * pc.scale;
    float height = perlin(scaledUV);
    vec3 normal = computeNormal(scaledUV, 0.001);

    // Pack normal [-1, 1] → [0, 1]
    vec3 packedNormal = normal * 0.5 + 0.5;

    // Output normals in RGB, height in A
    outColor = vec4(packedNormal, height);
}
