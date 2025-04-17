// src/AssetManagement/Meshes/PlaneGenerator.cpp

#include "PlaneGenerator.hpp"
#include "RenderEngine/RenderObjects/Materials.hpp"
#include "RenderEngine/RenderObjects/RenderObject.hpp"
#include "ResourceManagement/RenderResources/DescriptorBuffer.hpp"

#include <glm/fwd.hpp>

#include <vector>

assets::Mesh createPlane(ResourceManager* resourceManager, std::string materialPath) {
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

    Buffer vertexBuffer = resourceManager->createVertexBuffer(vertexSize).value();
    Buffer indexBuffer  = resourceManager->createIndexBuffer(indexSize).value();

    Buffer vertexStaging = resourceManager->createStagingBuffer(vertexSize).value();
    Buffer indexStaging  = resourceManager->createStagingBuffer(indexSize).value();

    std::memcpy(vertexStaging.info.pMappedData, vertices.data(), vertexSize);
    std::memcpy(indexStaging.info.pMappedData, indices.data(), indexSize);
    resourceManager->copyToBuffer(vertexStaging, vertexBuffer, vertexSize);
    resourceManager->copyToBuffer(indexStaging, indexBuffer, indexSize);

    vertexStaging.shutdown();
    indexStaging.shutdown();

    // HACK: Standardize please!
    Buffer materialBuffer = resourceManager->createUniformBuffer(256*2).value();
    DescriptorBuffer descriptor = resourceManager->createDescriptorBuffer(4).value();

    assets::Surface surface = {
        .indexStart = 0,
        .indexCount = static_cast<U32>(indices.size()),
        .materialIndex = 0,
    };

    assets::Mesh output = {
        .surfaces = {surface},
        .materials = {},
        .indexBuffer = indexBuffer,
        .vertexBuffer = vertexBuffer,
        .descriptor = descriptor,
        .materialBuffer = materialBuffer,
    };

    MaterialData matData = resourceManager->getMaterialManager()->getData(materialPath, &output.descriptor);
    output.materials = {matData};

    // Map buffers
    for (Size i = 0; i < matData.descriptorSets.size(); i++) {
        DescriptorSetData set = matData.descriptorSets[i];
        DescriptorLayoutInfo setInfo = matData.pipeline->descriptorLayouts[i];

        Size offset = 0;
        for (Size j = 0; j < set.bindings.size(); j++) {
            set.buffer->mapUniformBuffer(
                set.descriptorIndex,
                &materialBuffer,
                setInfo.bindings[j].size,
                offset
            );

            offset += setInfo.bindings[j].size;
        }
    }

    return output;
}

