// src/AssetManagement/Meshes/PlaneGenerator.cpp

#include "PlaneGenerator.hpp"
#include "RenderEngine/RenderObjects/Materials.hpp"
#include "RenderEngine/RenderObjects/RenderObject.hpp"
#include "ResourceManagement/RenderResources/DescriptorBuffer.hpp"

#include <glm/fwd.hpp>

#include <vector>

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

    output->descriptor.init(resourceManager->getVkInfo(), 4000);
    MaterialData matData = resourceManager->getMaterialManager()
        ->getData(materialPath, &output->descriptor);

    output->materials = {matData};

    output->surfaces = {{
        .indexStart = 0,
        .indexCount = static_cast<U32>(indices.size()),
        .materialIndex = 0,
    }};

    Image* clouds = resourceManager->loadImage("clouds.png");
    // Quick and dirty sampler creation
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.pNext = nullptr;
    samplerInfo.flags = 0;

    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;

    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = 1.0f;

    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;

    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = VK_LOD_CLAMP_NONE;
    samplerInfo.mipLodBias = 0.0f;

    VkSampler sampler;
    VkResult result = vkCreateSampler(
        resourceManager->getVkInfo()->device,
        &samplerInfo,
        nullptr,
        &sampler
    );
    if (result != VK_SUCCESS) {
        spdlog::error("Failed to create sampler!");
        return;
    }

    // Map buffers
    for (Size i = 0; i < matData.descriptorSets.size(); i++) {
        DescriptorSetData set = matData.descriptorSets[i];
        DescriptorSetInfo setInfo = matData.pipeline->descriptorSets[i];

        Size offset = 0;
        for (Size j = 0; j < set.bindings.size(); j++) {
            if (setInfo.bindings[j].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                set.buffer->mapUniformBuffer(
                    set.descriptorIndex,
                    set.bindings[j],
                    &output->materialBuffer,
                    setInfo.bindings[j].size,
                    offset
                );
            } else if (setInfo.bindings[j].descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
                set.buffer->mapImageSampler(
                    set.descriptorIndex,
                    set.bindings[j],
                    clouds,
                    sampler
                );
            }

            offset += setInfo.bindings[j].size;
        }
    }

    return;
}

