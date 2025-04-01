// src/AssetManagement/Meshes/PlaneGenerator.cpp

#include "PlaneGenerator.hpp"
#include "RenderEngine/RenderObjects/Materials.hpp"
#include "RenderEngine/RenderObjects/RenderObject.hpp"
#include "ResourceManagement/RenderResources/DescriptorBuffer.hpp"

#include <glm/fwd.hpp>

#include <vector>

assets::Mesh createPlane(ResourceManager* resourceManager, std::string materialPath) {

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

    // HACK: Standardize please!
    Buffer materialBuffer = resourceManager->createStorageBuffer(256*4).value();
    DescriptorBuffer descriptor = resourceManager->createDescriptorBuffer(100).value();
    MaterialData matData = resourceManager->getMaterialManager()->getData(materialPath, materialBuffer, descriptor);

    Buffer indexBuffer = resourceManager->createIndexBuffer(sizeof(U32) * indices.size()).value();
    Buffer vertexBuffer = resourceManager->createVertexBuffer(sizeof(Vertex) * vertices.size()).value();

    assets::Surface surface = {
        .indexStart = 0,
        .indexCount = 6,
        .materialIndex = 0,
    };

    assets::Mesh output = {
        .surfaces = {surface},
        .materials = {matData},
        .indexBuffer = indexBuffer,
        .vertexBuffer = vertexBuffer,
        .descriptor = descriptor,
        .materialBuffer = materialBuffer,
    };

    return output;
}

