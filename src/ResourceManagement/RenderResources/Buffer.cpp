// src/ResourceManagement/RenderResources/Buffer.hpp

#include "Buffer.hpp"

#include "RenderEngine/VkUtils.hpp"

#include <spdlog/spdlog.h>

#include <vector>

bool Buffer::init(
        std::shared_ptr<VulkanInfo> vkInfo,
        Size size,
        VkBufferUsageFlags bufferUsage,
        VmaMemoryUsage memoryUsage,
        VmaAllocationCreateFlags allocFlags
) {
    std::vector families = {vkInfo->graphicsQueueFamily, vkInfo->transferQueueFamily};

    VkBufferCreateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .size = size,
        .usage = bufferUsage,
        .sharingMode = VK_SHARING_MODE_CONCURRENT,
        // Shift to using Buffer Ownership Transfer with exclusing sharing for better performance
        .queueFamilyIndexCount = static_cast<U32>(families.size()),
        .pQueueFamilyIndices = families.data(),
    };

    VmaAllocationCreateInfo allocInfo = {
        .flags = allocFlags,
        .usage = memoryUsage,
        .requiredFlags = 0,
        .preferredFlags = 0,
        .memoryTypeBits = 0,
        .pool = nullptr,
        .pUserData = nullptr,
        .priority = 0,
    };

    VkResult res = vmaCreateBuffer(
            vkInfo->allocator,
            &bufferInfo,
            &allocInfo,
            &buffer,
            &allocation,
            &info);

    if (!VkUtils::checkVkResult(res, "Failed to create buffer")) {
        return false;
    }

    m_vkInfo = vkInfo;

    return true;
}

void Buffer::shutdown() {
    vmaDestroyBuffer(m_vkInfo->allocator, buffer, allocation);
    buffer = VK_NULL_HANDLE;
    allocation = VK_NULL_HANDLE;
    info = {};
}

void Buffer::map() {
    if (!info.pMappedData) {
        spdlog::warn("Attempted to map buffer twice!");
        return;
    }

    VkResult res = vmaMapMemory(m_vkInfo->allocator, allocation, &info.pMappedData);
    if (!VkUtils::checkVkResult(res, "Failed to map buffer memory")) {
        spdlog::error("This was assumed to never fail!");
        return;
    }
}

void Buffer::unmap() {
    if (info.pMappedData == nullptr) {
        spdlog::warn("Buffer is not mapped!");
        return;
    }

    vmaUnmapMemory(m_vkInfo->allocator, allocation);
    info.pMappedData = nullptr;
}

