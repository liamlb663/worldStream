// src/ResourceManagement/RenderResources/DescriptorBuffer.cpp

#include "DescriptorBuffer.hpp"

#include <spdlog/spdlog.h>

#include <cstring>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

// Static function pointers
PFN_vkCmdBindDescriptorBuffersEXT DescriptorBuffer::vkCmdBindDescriptorBuffersEXT = nullptr;
PFN_vkCmdSetDescriptorBufferOffsetsEXT DescriptorBuffer::vkCmdSetDescriptorBufferOffsetsEXT = nullptr;
PFN_vkGetDescriptorEXT DescriptorBuffer::vkGetDescriptorEXT = nullptr;

bool DescriptorBuffer::init(VulkanInfo* vkInfo, Size size) {
    m_vkInfo = vkInfo;

    // Set function pointers
    vkCmdBindDescriptorBuffersEXT = reinterpret_cast<PFN_vkCmdBindDescriptorBuffersEXT>(
        vkGetDeviceProcAddr(vkInfo->device, "vkCmdBindDescriptorBuffersEXT"));
    vkCmdSetDescriptorBufferOffsetsEXT = reinterpret_cast<PFN_vkCmdSetDescriptorBufferOffsetsEXT>(
        vkGetDeviceProcAddr(vkInfo->device, "vkCmdSetDescriptorBufferOffsetsEXT"));
    vkGetDescriptorEXT = reinterpret_cast<PFN_vkGetDescriptorEXT>(
        vkGetDeviceProcAddr(vkInfo->device, "vkGetDescriptorEXT"));

    if (!vkCmdBindDescriptorBuffersEXT || !vkCmdSetDescriptorBufferOffsetsEXT) {
        spdlog::error("Failed to load VK_EXT_descriptor_buffer functions");
        return false;
    }

    // Get physical device properties that apply to descriptor buffers HACK: this is a little hacky
    m_descriptorBufferProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_PROPERTIES_EXT;
    m_descriptorBufferProps.pNext = nullptr;

    VkPhysicalDeviceProperties2 props2 = {};
    props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    props2.pNext = &m_descriptorBufferProps;

    vkGetPhysicalDeviceProperties2(vkInfo->physicalDevice, &props2);

    // TODO: Use enum and props to determine descriptor size
    m_descriptorSize = m_descriptorBufferProps.uniformBufferDescriptorSize;

    VkDeviceSize alignment = m_descriptorBufferProps.descriptorBufferOffsetAlignment;
    if (m_descriptorSize % alignment != 0) {
        m_descriptorSize = ((m_descriptorSize + alignment - 1) / alignment) * alignment;
    }

    // Create buffer
    VkBufferUsageFlags bufferUsage =
        VK_BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT |
        VK_BUFFER_USAGE_TRANSFER_DST_BIT |
        VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

    VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO;

    VmaAllocationCreateFlags allocFlags =
        VMA_ALLOCATION_CREATE_MAPPED_BIT |
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;


    Size bufferSize = size * m_descriptorSize;

    bool bufferReturn = m_buffer.init(
            vkInfo,
            bufferSize,
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

U32 DescriptorBuffer::allocateBufferDescriptor(Buffer& buffer, Size range) {
    if (m_currentOffset + m_descriptorSize > m_buffer.info.size) {
        spdlog::error("Descriptor buffer overflow: tried to allocate beyond size");
        return -1;
    }

    VkDescriptorAddressInfoEXT addressInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_ADDRESS_INFO_EXT,
        .pNext = nullptr,
        .address = buffer.getAddress(),
        .range = range,
        .format = VK_FORMAT_UNDEFINED,
    };

    VkDescriptorGetInfoEXT getInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_GET_INFO_EXT,
        .pNext = nullptr,
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .data = {
            .pUniformBuffer = &addressInfo,
        }
    };

    if (!m_buffer.info.pMappedData) {
        spdlog::error("Descriptor buffer is not mapped!");
        return -1;
    }

    void* data = static_cast<char*>(m_buffer.info.pMappedData) + m_currentOffset;
    vkGetDescriptorEXT(m_vkInfo->device, &getInfo, m_descriptorBufferProps.uniformBufferDescriptorSize, data);

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

void DescriptorBuffer::verify(U32 descriptorIndex, U32 bindingIndex, const char* context) const {
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


