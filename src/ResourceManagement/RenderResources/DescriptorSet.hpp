// src/ResourceManagement/RenderResources/DescriptorSet.hpp

#pragma once

#include "RenderEngine/VulkanInfo.hpp"
#include "ResourceManagement/RenderResources/DescriptorPool.hpp"
#include "ResourceManagement/RenderResources/Buffer.hpp"
#include "ResourceManagement/RenderResources/Image.hpp"
#include "ResourceManagement/RenderResources/Sampler.hpp"
struct DescriptorSetInfo;

#include <vulkan/vulkan.h>
#include <vector>

class DescriptorSet {
public:
    bool init(VulkanInfo* vkInfo, VkDescriptorSet set);

    void writeUniformBuffer(U32 binding, Buffer* buffer, VkDeviceSize size, VkDeviceSize offset);
    void writeImageSampler(U32 binding, Image* image, Sampler sampler);
    void update();

    VkDescriptorSet get() const { return m_descriptorSet; }

    void bindBuffer(
        VkCommandBuffer commandBuffer,
        VkPipelineBindPoint pipelineBindPoint,
        VkPipelineLayout pipelineLayout,
        U32 setIndex
    ) {
        vkCmdBindDescriptorSets(
            commandBuffer,
            pipelineBindPoint,
            pipelineLayout,
            setIndex,
            1,
            &m_descriptorSet,
            0,
            nullptr
        );
    };

private:
    VulkanInfo* m_vkInfo = nullptr;
    DescriptorPool* m_pool = nullptr;
    VkDescriptorSet m_descriptorSet = VK_NULL_HANDLE;

    // Cached descriptor infos
    std::vector<VkWriteDescriptorSet> m_writes;
    std::vector<VkDescriptorBufferInfo> m_bufferInfos;
    std::vector<VkDescriptorImageInfo> m_imageInfos;
};
