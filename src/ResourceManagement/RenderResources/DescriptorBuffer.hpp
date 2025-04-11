// src/ResourceManagement/RenderResources/DescriptorBuffer.hpp

#pragma once

#include "Core/Types.hpp"
#include "RenderEngine/VulkanInfo.hpp"

#include "Buffer.hpp"
#include "Image.hpp"

#include <cstring>

class DescriptorBuffer {
public:
    bool init(VulkanInfo* vkInfo, Size size);
    void shutdown();

    U32 allocateSlot();
    void mapUniformBuffer(U32 index, Buffer* buffer, Size range, Size offset);
    void mapImageSampler(U32 index, Image* image, VkSampler sampler);

    void bindDescriptorBuffer(VkCommandBuffer commandBuffer);
    void bindDescriptorViaOffset(
        VkCommandBuffer commandBuffer,
        VkPipelineBindPoint pipelineBindPoint,
        VkPipelineLayout pipelineLayout,
        U32 setIndex,
        U32 bindingIndex,
        U32 descriptorIndex
    );

    void verify(U32 descriptorIndex, U32 bindingIndex, const char* context) const;

private:
    VulkanInfo* m_vkInfo;
    Buffer m_buffer;

    VkDeviceSize m_currentOffset = 0;
    VkDeviceSize m_descriptorSize = 0;

    VkPhysicalDeviceDescriptorBufferPropertiesEXT m_descriptorBufferProps = {};

    U32 indexToOffset(U32 index);

    static PFN_vkCmdBindDescriptorBuffersEXT vkCmdBindDescriptorBuffersEXT;
    static PFN_vkCmdSetDescriptorBufferOffsetsEXT vkCmdSetDescriptorBufferOffsetsEXT;
    static PFN_vkGetDescriptorEXT vkGetDescriptorEXT;
};

