// src/AssetManagement/Meshes/PlaneGenerator.cpp

#include "PlaneGenerator.hpp"
#include "AssetManagement/Meshes/Mesh.hpp"

void createPlane(
    ResourceManager* resourceManager,
    std::string materialPath,
    assets::Mesh* output,
    DescriptorPool* pool
) {
    *output = {};
    output->descriptor = pool;

    // Create Vertex Data
    std::vector<Vertex> vertices = {
        { {-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f} },
        { { 0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f} },
        { { 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f} },
        { {-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f} }
    };

    std::vector<U32> indices = {
        0, 1, 2,
        2, 3, 0
    };

    Size vertexSize = sizeof(Vertex) * vertices.size();
    Size indexSize  = sizeof(U32) * indices.size();

    output->vertexBuffer = resourceManager->createVertexBuffer(vertexSize).value();
    output->indexBuffer = resourceManager->createIndexBuffer(indexSize).value();

    Buffer vertexStaging = resourceManager->createStagingBuffer(vertexSize).value();
    Buffer indexStaging  = resourceManager->createStagingBuffer(indexSize).value();

    std::memcpy(vertexStaging.info.pMappedData, vertices.data(), vertexSize);
    std::memcpy(indexStaging.info.pMappedData, indices.data(), indexSize);
    resourceManager->copyToBuffer(vertexStaging, output->vertexBuffer, vertexSize);
    resourceManager->copyToBuffer(indexStaging, output->indexBuffer, indexSize);

    vertexStaging.shutdown();
    indexStaging.shutdown();

    // Use Surface Data
    output->surfaces = {{
        .indexStart = 0,
        .indexCount = static_cast<U32>(indices.size()),
        .materialIndex = 0,
    }};

    // Create Material Data
    MaterialData matData = resourceManager->getMaterialManager()->getData(
        materialPath, 
        output->descriptor
    );

    output->materials = {matData};
}

