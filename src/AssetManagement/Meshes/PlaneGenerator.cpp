// src/AssetManagement/Meshes/PlaneGenerator.cpp

#include "PlaneGenerator.hpp"
#include "AssetManagement/Meshes/Mesh.hpp"

void createPlane(
    ResourceManager* resourceManager,
    assets::Mesh* output,
    DescriptorPool* pool,
    U32 resolution
) {
    *output = {};
    output->descriptor = pool;

    std::vector<Vertex> vertices;
    std::vector<U32> indices;

    float step = 1.0f / (resolution - 1); // normalize to unit size (1.0)

    for (U32 y = 0; y < resolution; ++y) {
        for (U32 x = 0; x < resolution; ++x) {
            float fx = x * step;
            float fy = y * step;

            Vertex v;
            v.position = { fx - 0.5f, fy - 0.5f, 0.0f };     // centered on origin
            v.normal   = { 0.0f, 0.0f, 1.0f };               // facing +Z
            v.uv       = { fx, fy };                        // normalized UV

            vertices.push_back(v);
        }
    }

    for (U32 y = 0; y < resolution - 1; ++y) {
        for (U32 x = 0; x < resolution - 1; ++x) {
            U32 topLeft     = y * resolution + x;
            U32 topRight    = topLeft + 1;
            U32 bottomLeft  = (y + 1) * resolution + x;
            U32 bottomRight = bottomLeft + 1;

            // First triangle
            indices.push_back(topLeft);
            indices.push_back(topRight);
            indices.push_back(bottomLeft);

            // Second triangle
            indices.push_back(topRight);
            indices.push_back(bottomRight);
            indices.push_back(bottomLeft);
        }
    }

    output->vertexLayout = {
        .semantics = {"POSITION", "NORMAL", "TEXCOORD"},
        .formats = {
            {"POSITION", VK_FORMAT_R32G32B32_SFLOAT},
            {"NORMAL",   VK_FORMAT_R32G32B32_SFLOAT},
            {"TEXCOORD", VK_FORMAT_R32G32_SFLOAT},
        }
    };

    Size vertexSize = sizeof(Vertex) * vertices.size();
    Size indexSize  = sizeof(U32) * indices.size();

    output->vertexBuffer = resourceManager->createVertexBuffer(vertexSize).value();
    output->indexBuffer  = resourceManager->createIndexBuffer(indexSize).value();

    Buffer vertexStaging = resourceManager->createStagingBuffer(vertexSize).value();
    Buffer indexStaging  = resourceManager->createStagingBuffer(indexSize).value();

    std::memcpy(vertexStaging.info.pMappedData, vertices.data(), vertexSize);
    std::memcpy(indexStaging.info.pMappedData, indices.data(), indexSize);
    resourceManager->copyToBuffer(vertexStaging, output->vertexBuffer, vertexSize);
    resourceManager->copyToBuffer(indexStaging, output->indexBuffer, indexSize);

    vertexStaging.shutdown();
    indexStaging.shutdown();

    output->surfaces = {{
        .indexStart = 0,
        .indexCount = static_cast<U32>(indices.size()),
        .materialIndex = 0,
    }};
}

