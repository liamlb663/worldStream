// src/ResourceManagement/RenderResources/DescriptorBuffer.hpp

#pragma once

#include "Buffer.hpp"
#include "Core/Types.hpp"
#include "RenderEngine/VulkanInfo.hpp"
#include "spdlog/spdlog.h"
//#include "ResourceManagement/RenderResources/Image.hpp"

#include <cstring>

class DescriptorBuffer {
public:
    bool init(VulkanInfo* vkInfo, Size size);
    void shutdown();

    U32 allocateBufferDescriptor(const Buffer& buffer, Size range);
    //U32 allocateImageSamplerDescriptor(const Image& image, VkSampler sampler);

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

    static PFN_vkCmdBindDescriptorBuffersEXT vkCmdBindDescriptorBuffersEXT;
    static PFN_vkCmdSetDescriptorBufferOffsetsEXT vkCmdSetDescriptorBufferOffsetsEXT;
};

