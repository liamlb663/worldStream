// src/ResourceManagement/RenderResources/DescriptorSet.hpp

#pragma once

#include "RenderEngine/VulkanInfo.hpp"
#include "ResourceManagement/RenderResources/DescriptorPool.hpp"
#include "ResourceManagement/RenderResources/Buffer.hpp"
#include "ResourceManagement/RenderResources/Image.hpp"
#include "ResourceManagement/RenderResources/Sampler.hpp"
struct DescriptorSetInfo;

#include <vulkan/vulkan.h>

struct WriteEntry {
    VkWriteDescriptorSet write;
    VkDescriptorBufferInfo bufferInfo;
    VkDescriptorImageInfo imageInfo;
};

class DescriptorSet {
public:
    bool init(VulkanInfo* vkInfo, VkDescriptorSet set);

    void writeUniformBuffer(U32 binding, Buffer* buffer, VkDeviceSize size, VkDeviceSize offset);
    void writeImageSampler(U32 binding, Image* image, Sampler sampler);

    VkDescriptorSet get() const { return m_descriptorSet; }

    void bindBuffer(
        VkCommandBuffer commandBuffer,
        VkPipelineBindPoint pipelineBindPoint,
        VkPipelineLayout pipelineLayout,
        U32 setIndex
    );

private:
    VulkanInfo* m_vkInfo = nullptr;
    DescriptorPool* m_pool = nullptr;
    VkDescriptorSet m_descriptorSet = VK_NULL_HANDLE;
};
