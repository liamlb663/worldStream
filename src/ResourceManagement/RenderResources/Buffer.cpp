// src/ResourceManagement/RenderResources/Buffer.hpp

#include "Buffer.hpp"

#include "RenderEngine/VkUtils.hpp"

#include <vector>

bool Buffer::init(
        std::shared_ptr<VulkanInfo> vkInfo,
        Size size,
        VkBufferUsageFlags bufferUsage,
        VmaMemoryUsage memoryUsage
) {
    std::vector families = {vkInfo->graphicsQueueFamily, vkInfo->transferQueueFamily};

    VkBufferCreateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .size = size,
        .usage = bufferUsage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = static_cast<U32>(families.size()),
        .pQueueFamilyIndices = families.data(),
    };

    VmaAllocationCreateInfo allocInfo = {
        .flags = VMA_ALLOCATION_CREATE_MAPPED_BIT |
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
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
    this->size = size;

    return true;
}

void Buffer::shutdown() {
    vmaDestroyBuffer(m_vkInfo->allocator, buffer, allocation);
    size = 0;
}
