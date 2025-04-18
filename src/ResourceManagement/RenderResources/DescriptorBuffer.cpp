// src/ResourceManagement/RenderResources/DescriptorBuffer.cpp

#include "DescriptorBuffer.hpp"

#include <spdlog/spdlog.h>

#include "RenderEngine/RenderObjects/Materials.hpp"

#include <cstring>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

// Static function pointers
PFN_vkCmdBindDescriptorBuffersEXT DescriptorBuffer::vkCmdBindDescriptorBuffersEXT = nullptr;
PFN_vkCmdSetDescriptorBufferOffsetsEXT DescriptorBuffer::vkCmdSetDescriptorBufferOffsetsEXT = nullptr;
PFN_vkGetDescriptorEXT DescriptorBuffer::vkGetDescriptorEXT = nullptr;

bool DescriptorBuffer::init(VulkanInfo* vkInfo, Size size) {
    spdlog::info("init() on DescriptorBuffer: {}", static_cast<const void*>(this));

    m_vkInfo = vkInfo;

    // Set function pointers
    vkCmdBindDescriptorBuffersEXT = reinterpret_cast<PFN_vkCmdBindDescriptorBuffersEXT>(
        vkGetDeviceProcAddr(vkInfo->device, "vkCmdBindDescriptorBuffersEXT"));
    vkCmdSetDescriptorBufferOffsetsEXT = reinterpret_cast<PFN_vkCmdSetDescriptorBufferOffsetsEXT>(
        vkGetDeviceProcAddr(vkInfo->device, "vkCmdSetDescriptorBufferOffsetsEXT"));
    vkGetDescriptorEXT = reinterpret_cast<PFN_vkGetDescriptorEXT>(
        vkGetDeviceProcAddr(vkInfo->device, "vkGetDescriptorEXT"));

    if (!vkCmdBindDescriptorBuffersEXT || !vkCmdSetDescriptorBufferOffsetsEXT || !vkGetDescriptorEXT) {
        spdlog::error("Failed to load VK_EXT_descriptor_buffer functions");
        return false;
    }

    // Get physical device properties that apply to descriptor buffers
    props = {};
    props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_PROPERTIES_EXT;
    props.pNext = nullptr;
    VkPhysicalDeviceProperties2 props2 = {};
    props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    props2.pNext = &props;
    vkGetPhysicalDeviceProperties2(vkInfo->physicalDevice, &props2);

    m_alignment = props.descriptorBufferOffsetAlignment;

    // Create buffer
    VkBufferUsageFlags bufferUsage =
        VK_BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT |
        VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
        VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO;

    VmaAllocationCreateFlags allocFlags =
        VMA_ALLOCATION_CREATE_MAPPED_BIT |
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

    return m_buffer.init(
            vkInfo,
            size,
            bufferUsage,
            memoryUsage,
            allocFlags
    );
};

void DescriptorBuffer::shutdown() {
    spdlog::info("shutdown() on DescriptorBuffer: {}", static_cast<const void*>(this));

    m_buffer.shutdown();
    m_sets.clear();
    m_currentOffset = 0;
}

U32 DescriptorBuffer::allocateSlot(DescriptorSetInfo* info) {
    spdlog::info("allocateSlot() on DescriptorBuffer: {}", static_cast<const void*>(this));
    spdlog::info("Allocating descriptor slot for layout with {} bindings", info->bindings.size());

    VkDeviceSize baseOffset = (m_currentOffset + m_alignment - 1) & ~(m_alignment - 1);
    VkDeviceSize setSize = 0;

    SetSlot slot = {
        .info = info,
        .size = 0,
        .offset = 0,
        .bindingTypes = {},
        .bindingOffsets = {},
    };

    for (const auto& binding : info->bindings) {
        VkDeviceSize align = m_alignment;
        VkDeviceSize bindingSize = calculateDescriptorSize(binding.descriptorType);

        // Align each binding individually if required
        setSize = (setSize + align - 1) & ~(align - 1);

        slot.bindingOffsets[binding.binding] = setSize;
        slot.bindingTypes[binding.binding] = binding.descriptorType;

        setSize += bindingSize;
    }

    slot.offset = baseOffset;
    slot.size = setSize;
    m_currentOffset = baseOffset + setSize;

    spdlog::info("Set size to allocate: {}", setSize);
    if (setSize == 0) {
        spdlog::error("Descriptor set has zero size! Descriptor types might be unsupported.");
        for (const auto& binding : info->bindings) {
            spdlog::error("  Binding {} has type {}", binding.binding, string_VkDescriptorType(binding.descriptorType));
            spdlog::error("  Calculated size: {}", calculateDescriptorSize(binding.descriptorType));
        }
    }

    m_sets.push_back(slot);
    spdlog::info("Set slot pushed. Total sets now: {}", m_sets.size());

    return static_cast<U32>(m_sets.size() - 1);
};

void DescriptorBuffer::mapUniformBuffer(U32 setID, U32 binding, Buffer* buffer, Size range, Size offset) {
    SetSlot& set = m_sets[setID];

    VkDescriptorAddressInfoEXT addressInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_ADDRESS_INFO_EXT,
        .pNext = nullptr,
        .address = buffer->getAddress() + offset,
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
        return;
    }

    void* data = static_cast<char*>(m_buffer.info.pMappedData) + set.offset + set.bindingOffsets.at(binding);
    VkDeviceSize size = calculateDescriptorSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    vkGetDescriptorEXT(m_vkInfo->device, &getInfo, size, data);
}

void DescriptorBuffer::mapImageSampler(U32 setID, U32 binding, Image* image, VkSampler sampler) {
    const auto& set = m_sets[setID];

    VkDescriptorImageInfo imageInfo = {
        .sampler = sampler,
        .imageView = image->view,
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    };

    VkDescriptorGetInfoEXT getInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_GET_INFO_EXT,
        .pNext = nullptr,
        .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .data = {
            .pCombinedImageSampler = &imageInfo
        }
    };

    void* dst = static_cast<char*>(m_buffer.info.pMappedData) + set.offset + set.bindingOffsets.at(binding);
    VkDeviceSize size = calculateDescriptorSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    vkGetDescriptorEXT(m_vkInfo->device, &getInfo, size, dst);
}

void DescriptorBuffer::bindDescriptorBuffer(VkCommandBuffer commandBuffer) {
    VkDescriptorBufferBindingInfoEXT bindingInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_BUFFER_BINDING_INFO_EXT,
        .pNext = nullptr,
        .address = m_buffer.getAddress(),
        .usage = VK_BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT,
    };

    vkCmdBindDescriptorBuffersEXT(commandBuffer, 1, &bindingInfo);
}

void DescriptorBuffer::bindDescriptorViaOffset(
    VkCommandBuffer commandBuffer,
    VkPipelineBindPoint pipelineBindPoint,
    VkPipelineLayout pipelineLayout,
    U32 setIndex,
    U32 descriptorSetID
) {
    spdlog::info("bindDescriptorViaOffset() on DescriptorBuffer: {}", static_cast<const void*>(this));
    spdlog::info("bindDescriptorViaOffset() m_sets.size(): {}", m_sets.size());
    if (descriptorSetID >= m_sets.size()) {
        spdlog::error("Invalid descriptorSetID {}. Only {} descriptor sets allocated.", descriptorSetID, m_sets.size());
        return;
    }

    VkDeviceSize offset = m_sets[descriptorSetID].offset;

    spdlog::info("Binding setIndex={}, descriptorSetID={}, resolvedOffset={}",
                 setIndex, descriptorSetID, offset);

    vkCmdSetDescriptorBufferOffsetsEXT(commandBuffer, pipelineBindPoint, pipelineLayout,
                                       setIndex, 1, &setIndex, &offset);
}

void DescriptorBuffer::verify(U32 setID, U32 binding, const char* context) const {
    const auto& set = m_sets[setID];
    VkDeviceSize descriptorOffset = set.offset + set.bindingOffsets.at(binding);
    VkDeviceSize descriptorSize = calculateDescriptorSize(set.bindingTypes.at(binding));

    if (descriptorOffset % m_alignment != 0) {
        spdlog::warn("[{}] Descriptor offset {} not aligned to {}", context, descriptorOffset, m_alignment);
    }

    if (descriptorOffset + descriptorSize > m_buffer.info.size) {
        spdlog::error("[{}] Descriptor offset {} + size {} exceeds buffer size {}", context, descriptorOffset, descriptorSize, m_buffer.info.size);
    } else {
        spdlog::info("[{}] Descriptor verified at offset {} (size = {})", context, descriptorOffset, descriptorSize);
    }
}

VkDeviceSize DescriptorBuffer::calculateDescriptorSize(VkDescriptorType type) const {
    switch (type) {
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER: return props.uniformBufferDescriptorSize;
        case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE: return props.sampledImageDescriptorSize;
        case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: return props.combinedImageSamplerDescriptorSize;
        case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER: return props.storageBufferDescriptorSize;
        default:
            spdlog::error("Unsupported descriptor type in descriptorSize(): {}", 
                          string_VkDescriptorType(type));
            return 0;
    }
}


