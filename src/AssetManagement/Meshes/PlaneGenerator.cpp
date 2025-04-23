// src/AssetManagement/Meshes/PlaneGenerator.cpp

#include "PlaneGenerator.hpp"

void createPlane(ResourceManager* resourceManager, std::string materialPath, assets::Mesh* output) {
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

    // HACK: Standardize please!
    output->materialBuffer = resourceManager->createUniformBuffer(256*2).value();

    std::array<DescriptorPool::PoolSizeRatio, 2> poolRatios = {{
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2.0f },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1.0f }
    }};

    output->descriptor = resourceManager->createDescriptorPool(3, poolRatios).value();
    MaterialData matData = resourceManager->getMaterialManager()
        ->getData(materialPath, &output->descriptor);

    output->materials = {matData};

    output->surfaces = {{
        .indexStart = 0,
        .indexCount = static_cast<U32>(indices.size()),
        .materialIndex = 0,
    }};

    Image* clouds = resourceManager->loadImage("clouds.png");

    Sampler sampler = resourceManager->getSamplerBuilder()
        .setFilter(VK_FILTER_LINEAR, VK_FILTER_LINEAR)
        .setAddressMode(
            VK_SAMPLER_ADDRESS_MODE_REPEAT,
            VK_SAMPLER_ADDRESS_MODE_REPEAT,
            VK_SAMPLER_ADDRESS_MODE_REPEAT)
        .disableAnisotropy()
        .setBorderColor(VK_BORDER_COLOR_INT_OPAQUE_BLACK)
        .setUnnormalizedCoords(false)
        .setCompareOp(VK_COMPARE_OP_ALWAYS)
        .setLod(0.0f, VK_LOD_CLAMP_NONE)
        .build()
        .value();

    // Map buffers
    for (Size i = 0; i < matData.descriptorSets.size(); i++) {
        DescriptorSetData set = matData.descriptorSets[i];
        DescriptorSetInfo setInfo = matData.pipeline->descriptorSets[i];

        Size offset = 0;
        for (Size j = 0; j < set.bindings.size(); j++) {
            if (setInfo.bindings[j].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                set.set.writeUniformBuffer(
                    set.bindings[j],
                    &output->materialBuffer,
                    setInfo.bindings[j].size,
                    offset
                );
            } else if (setInfo.bindings[j].descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
                set.set.writeImageSampler(
                    set.bindings[j],
                    clouds,
                    sampler
                );
            }

            set.set.update();

            offset += setInfo.bindings[j].size;
        }
    }

    return;
}

