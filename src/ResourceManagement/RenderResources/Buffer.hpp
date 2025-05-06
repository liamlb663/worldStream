// src/ResourceManagement/RenderResources/Buffer.hpp

#pragma once

#include "RenderEngine/VulkanInfo.hpp"

#include <string>
#include <vulkan/vulkan.h>

class Buffer {
public:
    VkBuffer buffer = VK_NULL_HANDLE;
    VmaAllocation allocation = VK_NULL_HANDLE;
    VmaAllocationInfo info = {};
    VkDeviceAddress address = 0;

    bool init(
            VulkanInfo* vkInfo,
            Size size,
            VkBufferUsageFlags bufferUsage,
            VmaMemoryUsage memoryUsage,
            VmaAllocationCreateFlags allocFlags,
            std::string name
    );

    void shutdown();

    void map();
    void unmap();

    VkDeviceAddress getAddress();

private:
    VulkanInfo* m_vkInfo;

};

