// src/AssetManagement/Meshes/CubeGenerator.cpp

#include "CubeGenerator.hpp"
#include "spdlog/spdlog.h"
#include <cstring>

void createCube(
    ResourceManager* resourceManager,
    std::string materialPath,
    assets::Mesh* output,
    DescriptorPool* pool
) {
    *output = {};
    output->descriptor = pool;

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    // Define vertices for a unit cube (centered at origin)
    std::vector<Vertex> vertices = {
        // Front (+Y)
        {{-0.5f,  0.5f, -0.5f}, {0, 1, 0}, {0, 0}},
        {{ 0.5f,  0.5f, -0.5f}, {0, 1, 0}, {1, 0}},
        {{ 0.5f,  0.5f,  0.5f}, {0, 1, 0}, {1, 1}},
        {{-0.5f,  0.5f,  0.5f}, {0, 1, 0}, {0, 1}},

        // Back (−Y)
        {{ 0.5f, -0.5f, -0.5f}, {0, -1, 0}, {0, 0}},
        {{-0.5f, -0.5f, -0.5f}, {0, -1, 0}, {1, 0}},
        {{-0.5f, -0.5f,  0.5f}, {0, -1, 0}, {1, 1}},
        {{ 0.5f, -0.5f,  0.5f}, {0, -1, 0}, {0, 1}},

        // Left (−X)
        {{-0.5f, -0.5f, -0.5f}, {-1, 0, 0}, {0, 0}},
        {{-0.5f,  0.5f, -0.5f}, {-1, 0, 0}, {1, 0}},
        {{-0.5f,  0.5f,  0.5f}, {-1, 0, 0}, {1, 1}},
        {{-0.5f, -0.5f,  0.5f}, {-1, 0, 0}, {0, 1}},

        // Right (+X)
        {{ 0.5f,  0.5f, -0.5f}, {1, 0, 0}, {0, 0}},
        {{ 0.5f, -0.5f, -0.5f}, {1, 0, 0}, {1, 0}},
        {{ 0.5f, -0.5f,  0.5f}, {1, 0, 0}, {1, 1}},
        {{ 0.5f,  0.5f,  0.5f}, {1, 0, 0}, {0, 1}},

        // Top (+Z)
        {{-0.5f, -0.5f,  0.5f}, {0, 0, 1}, {0, 0}},
        {{ 0.5f, -0.5f,  0.5f}, {0, 0, 1}, {1, 0}},
        {{ 0.5f,  0.5f,  0.5f}, {0, 0, 1}, {1, 1}},
        {{-0.5f,  0.5f,  0.5f}, {0, 0, 1}, {0, 1}},

        // Bottom (−Z)
        {{-0.5f, -0.5f, -0.5f}, {0, 0, -1}, {0, 0}},
        {{ 0.5f, -0.5f, -0.5f}, {0, 0, -1}, {1, 0}},
        {{ 0.5f,  0.5f, -0.5f}, {0, 0, -1}, {1, 1}},
        {{-0.5f,  0.5f, -0.5f}, {0, 0, -1}, {0, 1}},
    };

    std::vector<U32> indices = {
        // Front (+Y)
        0, 1, 2, 2, 3, 0,
        // Back (−Y)
        4, 5, 6, 6, 7, 4,
        // Left (−X)
        8, 9,10,10,11, 8,
        // Right (+X)
       12,13,14,14,15,12,
        // Top (+Z)
       16,17,18,18,19,16,
        // Bottom (−Z)
       20,21,22,22,23,20
    };

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

    MaterialData matData = resourceManager->getMaterialManager()->getData(
        materialPath,
        output->descriptor,
        &output->vertexLayout
    );

    output->materials = {matData};
}

