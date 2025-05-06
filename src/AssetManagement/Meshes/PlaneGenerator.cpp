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

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
        glm::vec3 tangent;
        glm::vec3 bitangent;
    };

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
            v.tangent = glm::vec3(0.0f);
            v.bitangent = glm::vec3(0.0f);

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

    // Compute tangents and bitangents
    for (Size i = 0; i < indices.size(); i += 3) {
        Vertex& v0 = vertices[indices[i + 0]];
        Vertex& v1 = vertices[indices[i + 1]];
        Vertex& v2 = vertices[indices[i + 2]];

        glm::vec3 edge1 = v1.position - v0.position;
        glm::vec3 edge2 = v2.position - v0.position;
        glm::vec2 deltaUV1 = v1.uv - v0.uv;
        glm::vec2 deltaUV2 = v2.uv - v0.uv;

        F32 f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        glm::vec3 tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
        glm::vec3 bitangent = f * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);

        v0.tangent += tangent;
        v1.tangent += tangent;
        v2.tangent += tangent;

        v0.bitangent += bitangent;
        v1.bitangent += bitangent;
        v2.bitangent += bitangent;
    }

    output->vertexLayout = {
        .semantics = {"POSITION", "NORMAL", "TEXCOORD", "TANGENT", "BITANGENT"},
        .formats = {
            {"POSITION", VK_FORMAT_R32G32B32_SFLOAT},
            {"NORMAL",   VK_FORMAT_R32G32B32_SFLOAT},
            {"TEXCOORD", VK_FORMAT_R32G32_SFLOAT},
            {"TANGENT",    VK_FORMAT_R32G32B32_SFLOAT},
            {"BITANGENT",  VK_FORMAT_R32G32B32_SFLOAT},
        }
    };

    Size vertexSize = sizeof(Vertex) * vertices.size();
    Size indexSize  = sizeof(U32) * indices.size();

    output->vertexBuffer = resourceManager->createVertexBuffer(vertexSize, "Plane Vertex Buffer").value();
    output->indexBuffer  = resourceManager->createIndexBuffer(indexSize, "Plane Index Buffer").value();

    Buffer vertexStaging = resourceManager->createStagingBuffer(vertexSize, "Plane Vertex Staging Buffer").value();
    Buffer indexStaging  = resourceManager->createStagingBuffer(indexSize, "Plane Index Staging Buffer").value();

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

