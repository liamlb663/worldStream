// src/AssetManagement/Meshes/PlaneGenerator.cpp

#include "PlaneGenerator.hpp"
#include "RenderEngine/RenderObjects/Materials.hpp"
#include "RenderEngine/RenderObjects/RenderObject.hpp"
#include "ResourceManagement/RenderResources/DescriptorBuffer.hpp"

#include <glm/fwd.hpp>

#include <vector>

assets::Mesh createPlane(ResourceManager* resourceManager, std::string materialPath) {
    assets::Mesh output = {};

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

    output.vertexBuffer = resourceManager->createVertexBuffer(vertexSize).value();
    output.indexBuffer = resourceManager->createIndexBuffer(indexSize).value();

    Buffer vertexStaging = resourceManager->createStagingBuffer(vertexSize).value();
    Buffer indexStaging  = resourceManager->createStagingBuffer(indexSize).value();

    std::memcpy(vertexStaging.info.pMappedData, vertices.data(), vertexSize);
    std::memcpy(indexStaging.info.pMappedData, indices.data(), indexSize);
    resourceManager->copyToBuffer(vertexStaging, output.vertexBuffer, vertexSize);
    resourceManager->copyToBuffer(indexStaging, output.indexBuffer, indexSize);

    vertexStaging.shutdown();
    indexStaging.shutdown();

    // HACK: Standardize please!
    output.materialBuffer = resourceManager->createUniformBuffer(256*2).value();
    //output.descriptor = resourceManager->createDescriptorBuffer(4000).value();

    DescriptorBuffer descriptor = resourceManager->createDescriptorBuffer(4000).value();
    MaterialData matData = resourceManager->getMaterialManager()->getData(materialPath, &descriptor);
    output.descriptor = descriptor; // assign after allocation + init

    output.surfaces = {{
        .indexStart = 0,
        .indexCount = static_cast<U32>(indices.size()),
        .materialIndex = 0,
    }};

    //MaterialData matData = resourceManager->getMaterialManager()->getData(materialPath, &output.descriptor);
    //output.materials = {matData};

    // Map buffers
    for (Size i = 0; i < matData.descriptorSets.size(); i++) {
        DescriptorSetData set = matData.descriptorSets[i];
        DescriptorSetInfo setInfo = matData.pipeline->descriptorSets[i];

        Size offset = 0;
        for (Size j = 0; j < set.bindings.size(); j++) {
            set.buffer->mapUniformBuffer(
                set.descriptorIndex,
                set.bindings[j],
                &output.materialBuffer,
                setInfo.bindings[j].size,
                offset
            );

            offset += setInfo.bindings[j].size;
        }
    }

    return output;
}

