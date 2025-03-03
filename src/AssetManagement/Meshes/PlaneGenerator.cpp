// src/AssetManagement/Meshes/PlaneGenerator.cpp

#include "PlaneGenerator.hpp"
#include "RenderEngine/RenderObjects/RenderObject.hpp"
#include "ResourceManagement/RenderResources/DescriptorBuffer.hpp"

#include <glm/fwd.hpp>

#include <vector>

assets::Mesh createPlane(ResourceManager* resourceManager) {

    std::vector<Vertex> vertices = {
        { glm::vec3(-0.5f, -0.5f, 0.0f), 0.0f, glm::vec3(0.0f, 0.0f, 1.0f), 0.0f },
        { glm::vec3( 0.5f, -0.5f, 0.0f), 1.0f, glm::vec3(0.0f, 0.0f, 1.0f), 0.0f },
        { glm::vec3( 0.5f,  0.5f, 0.0f), 1.0f, glm::vec3(0.0f, 0.0f, 1.0f), 1.0f },
        { glm::vec3(-0.5f,  0.5f, 0.0f), 0.0f, glm::vec3(0.0f, 0.0f, 1.0f), 1.0f },
    };

    // Indices for two triangles forming the quad
    std::vector<U32> indices = {
        0, 1, 2,  // First triangle
        2, 3, 0   // Second triangle
    };

    resourceManager->getMaterialManager();

    Buffer indexBuffer = resourceManager->createIndexBuffer(sizeof(U32) * indices.size()).value();
    Buffer vertexBuffer = resourceManager->createVertexBuffer(sizeof(Vertex) * vertices.size()).value();

    assets::Surface surface = {
        .indexStart = 0,
        .indexCount = 6,
        .materialIndex = 0,
    };

    DescriptorBuffer descriptor = resourceManager->createDescriptorBuffer(1).value();
    Buffer materialBuffer = resourceManager->createStorageBuffer(1).value();

    assets::Mesh output = {
        .surfaces = {surface},
        .materials = {},        // TODO: Thinking this'll just be an input or string input
        .indexBuffer = indexBuffer,
        .vertexBuffer = vertexBuffer,
        .descriptor = descriptor,
        .materialBuffer = materialBuffer,
    };

    return {};
}

