// src/ResourceManagement/RenderResources/DescriptorSet.hpp

#pragma once

#include "RenderEngine/VulkanInfo.hpp"
#include "RenderEngine/RenderObjects/Materials.hpp"
#include "ResourceManagement/RenderResources/DescriptorPool.hpp"
#include "ResourceManagement/RenderResources/Buffer.hpp"
#include "ResourceManagement/RenderResources/Image.hpp"

#include <vulkan/vulkan.h>
#include <vector>

class DescriptorSet {
public:
    bool init(VulkanInfo* vkInfo, DescriptorPool* pool, const DescriptorSetInfo& setInfo);

    void writeUniformBuffer(U32 binding, Buffer* buffer, VkDeviceSize size, VkDeviceSize offset = 0);
    void writeImageSampler(U32 binding, Image* image, VkSampler sampler);
    void update();

    VkDescriptorSet get() const { return m_descriptorSet; }

private:
    VulkanInfo* m_vkInfo = nullptr;
    DescriptorPool* m_pool = nullptr;
    VkDescriptorSet m_descriptorSet = VK_NULL_HANDLE;
    DescriptorSetInfo m_setInfo;

    // Cached descriptor infos
    std::vector<VkWriteDescriptorSet> m_writes;
    std::vector<VkDescriptorBufferInfo> m_bufferInfos;
    std::vector<VkDescriptorImageInfo> m_imageInfos;
};
