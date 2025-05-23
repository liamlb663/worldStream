#version 450

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

// Global Data
layout(set = 0, binding = 0) uniform GlobalUBO {
    mat4 view;
    mat4 proj;
    vec3 cameraPosition;
    float time;
};

layout(set = 0, binding = 1) uniform LightUBO {
    vec3 directionalLightDir;
    float _pad2;
    vec3 directionalLightColor;
    float intensity;
};

void main() {
    const float stoneCoefficient = 0.55; // Lower = more grass?

    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize(-directionalLightDir); // light coming *toward* the surface
    vec3 viewDir = normalize(cameraPosition - fragPos);

    // Lighting calculations
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * directionalLightColor;

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * directionalLightColor;

    float specularStrength = 0.5;
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * directionalLightColor;

    // Combine lighting
    vec3 lightColor = (ambient + diffuse + specular) * intensity;

    // Slope-based blending
    float slope = dot(normal, vec3(0.0, 0.0, 1.0));
    slope = clamp((slope - stoneCoefficient) / (1.0 - stoneCoefficient), 0.0, 1.0);

    // Define base colors
    vec3 grassColor = vec3(0.2, 0.6, 0.2);
    vec3 stoneColor = vec3(0.5, 0.5, 0.5);

    // Blend based on slope (flat = grass, steep = stone)
    vec3 surfaceColor = mix(stoneColor, grassColor, slope);
    // Apply lighting
    vec3 finalColor = surfaceColor * lightColor;

    outColor = vec4(finalColor, 1.0);

    // Visualize normals (in 0-1 range)
    //outColor = vec4(normal * 0.5 + 0.5, 1.0);

    // Visualize UV coordinates
    //outColor = vec4(fragUV, 0.0, 1.0);

    // Visualize slope value as grayscale
    //outColor = vec4(vec3(slope), 1.0);

    // Visualize height by mapping fragPos.z (if height matters)
    //float heightNormalized = clamp((fragPos.z + 10.0) / 20.0, 0.0, 1.0);
    //outColor = vec4(vec3(heightNormalized), 1.0);
}
