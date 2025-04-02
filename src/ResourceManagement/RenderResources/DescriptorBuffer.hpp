// src/ResourceManagement/RenderResources/DescriptorBuffer.hpp

#pragma once

#include "Buffer.hpp"
#include "Core/Types.hpp"
#include "RenderEngine/VulkanInfo.hpp"
#include "spdlog/spdlog.h"
//#include "ResourceManagement/RenderResources/Image.hpp"

#include <cstring>
#include <memory>

class DescriptorBuffer {
public:
    bool init(std::shared_ptr<VulkanInfo> vkInfo, Size size);
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

void verifyDescriptorBinding(U32 descriptorIndex, U32 bindingIndex, const char* context) const {
    if (!m_buffer.info.pMappedData) {
        spdlog::error("[{}] DescriptorBuffer is not mapped!", context);
        return;
    }

    if (m_descriptorSize == 0) {
        spdlog::error("[{}] Descriptor size is zero!", context);
        return;
    }

    VkDeviceSize descriptorOffset = descriptorIndex * m_descriptorSize;

    // Check if offset is aligned
    VkDeviceSize alignment = m_descriptorBufferProps.descriptorBufferOffsetAlignment;
    if (descriptorOffset % alignment != 0) {
        spdlog::warn("[{}] Descriptor offset {} is not aligned to {}", context, descriptorOffset, alignment);
    }

    // Check if offset + size would overflow buffer
    if (descriptorOffset + m_descriptorSize > m_buffer.info.size) {
        spdlog::error("[{}] Descriptor offset {} + size {} exceeds buffer size {}!",
                      context, descriptorOffset, m_descriptorSize, m_buffer.info.size);
    }

    // Optional: log detailed info for cross-checking
    spdlog::info("[{}] Verifying descriptor binding:", context);
    spdlog::info("\tdescriptorIndex: {}", descriptorIndex);
    spdlog::info("\tdescriptorOffset: {}", descriptorOffset);
    spdlog::info("\tdescriptorSize: {}", m_descriptorSize);
    spdlog::info("\tbufferSize: {}", m_buffer.info.size);
    spdlog::info("\tbindingIndex: {}", bindingIndex);
    spdlog::info("\toffset alignment requirement: {}", alignment);
}

private:
    std::shared_ptr<VulkanInfo> m_vkInfo;
    Buffer m_buffer;

    bool initited = false;
    VkDeviceSize m_currentOffset = 0;
    VkDeviceSize m_descriptorSize = 0;

    VkPhysicalDeviceDescriptorBufferPropertiesEXT m_descriptorBufferProps = {};

    static PFN_vkCmdBindDescriptorBuffersEXT vkCmdBindDescriptorBuffersEXT;
    static PFN_vkCmdSetDescriptorBufferOffsetsEXT vkCmdSetDescriptorBufferOffsetsEXT;
};

