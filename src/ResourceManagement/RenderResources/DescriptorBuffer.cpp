// src/ResourceManagement/RenderResources/DescriptorSet.cpp

#include "DescriptorBuffer.hpp"

#include <spdlog/spdlog.h>

#include <cstring>
#include <vulkan/vulkan.h>

// Static function pointers
PFN_vkCmdBindDescriptorBuffersEXT DescriptorBuffer::vkCmdBindDescriptorBuffersEXT = nullptr;
PFN_vkCmdSetDescriptorBufferOffsetsEXT DescriptorBuffer::vkCmdSetDescriptorBufferOffsetsEXT = nullptr;

bool DescriptorBuffer::init(std::shared_ptr<VulkanInfo> vkInfo, Size size) {
    m_vkInfo = vkInfo;

    vkCmdBindDescriptorBuffersEXT = reinterpret_cast<PFN_vkCmdBindDescriptorBuffersEXT>(
        vkGetDeviceProcAddr(vkInfo->device, "vkCmdBindDescriptorBuffersEXT"));
    vkCmdSetDescriptorBufferOffsetsEXT = reinterpret_cast<PFN_vkCmdSetDescriptorBufferOffsetsEXT>(
        vkGetDeviceProcAddr(vkInfo->device, "vkCmdSetDescriptorBufferOffsetsEXT"));

    if (!vkCmdBindDescriptorBuffersEXT || !vkCmdSetDescriptorBufferOffsetsEXT) {
        spdlog::error("Failed to load VK_EXT_descriptor_buffer functions");
        return false;
    }

    VkBufferUsageFlags bufferUsage =
        VK_BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT |
        VK_BUFFER_USAGE_TRANSFER_DST_BIT |
        VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

    VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO;

    VmaAllocationCreateFlags allocFlags =
        VMA_ALLOCATION_CREATE_MAPPED_BIT;

    bool bufferReturn = m_buffer.init(
            vkInfo,
            size,
            bufferUsage,
            memoryUsage,
            allocFlags
    );

    if (!bufferReturn) {
        return false;
    }

    m_currentOffset = 0;
    return true;
};

void DescriptorBuffer::shutdown() {
    m_buffer.shutdown();
}

U32 DescriptorBuffer::allocateBufferDescriptor(const Buffer& buffer, Size range) {
    m_descriptorSize = sizeof(VkDescriptorBufferInfo);

    VkDescriptorBufferInfo info = {
        .buffer = buffer.buffer,
        .offset = 0,
        .range = range,
    };

    if (m_currentOffset + m_descriptorSize > m_buffer.info.size) {
        return -1;
    }

    void* data = static_cast<char*>(m_buffer.info.pMappedData) + m_currentOffset;
    std::memcpy(data, &info, sizeof(VkDescriptorBufferInfo));

    U32 descriptorIndex = static_cast<uint32_t>(m_currentOffset / m_descriptorSize);
    m_currentOffset += m_descriptorSize;
    return descriptorIndex;
}

void DescriptorBuffer::bindDescriptorBuffer(VkCommandBuffer commandBuffer) {
    VkDescriptorBufferBindingInfoEXT bindingInfo = {};
    bindingInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_BUFFER_BINDING_INFO_EXT;
    bindingInfo.address = m_buffer.getAddress();
    bindingInfo.usage = VK_BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT;

    vkCmdBindDescriptorBuffersEXT(commandBuffer, 1, &bindingInfo);
}

void DescriptorBuffer::bindDescriptorViaOffset(
        VkCommandBuffer commandBuffer,
        VkPipelineBindPoint pipelineBindPoint,
        VkPipelineLayout pipelineLayout,
        uint32_t setIndex,
        uint32_t bindingIndex,
        uint32_t descriptorIndex
) {
    VkDeviceSize descriptorOffset = descriptorIndex * m_descriptorSize;

    vkCmdSetDescriptorBufferOffsetsEXT(
        commandBuffer,
        pipelineBindPoint,
        pipelineLayout,
        setIndex,
        1,                // Number of bindings
        &bindingIndex,    // Binding index in the set
        &descriptorOffset // Offset of the descriptor in the buffer
    );
}

