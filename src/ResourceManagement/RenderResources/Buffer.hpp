// src/ResourceManagement/RenderResources/Buffer.hpp

#pragma once

#include "RenderEngine/VulkanInfo.hpp"

#include <vulkan/vulkan.h>

#include <memory>

class Buffer {
public:
    VkBuffer buffer = VK_NULL_HANDLE;
    VmaAllocation allocation = VK_NULL_HANDLE;
    VmaAllocationInfo info = {};
    VkDeviceAddress address = 0;

    bool init(
            std::shared_ptr<VulkanInfo> vkInfo,
            Size size,
            VkBufferUsageFlags bufferUsage,
            VmaMemoryUsage memoryUsage,
            VmaAllocationCreateFlags allocFlags
    );

    void shutdown();

    void map();
    void unmap();

    VkDeviceAddress getAddress();

private:
    std::shared_ptr<VulkanInfo> m_vkInfo;

};

