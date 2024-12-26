// src/ResourceManagement/RenderResources/Buffer.hpp

#pragma once

#include "RenderEngine/VulkanInfo.hpp"

#include <vulkan/vulkan.h>

#include <memory>

class Buffer {
public:
    VkBuffer buffer;
    VmaAllocation allocation;
    VmaAllocationInfo info;
    Size size;

    bool init(
            std::shared_ptr<VulkanInfo> vkInfo,
            Size size,
            VkBufferUsageFlags bufferUsage,
            VmaMemoryUsage memoryUsage
    );

    void shutdown();

    //bool updateData(const void* data, Size size, Size offset);
    //void copyTo(VkCommandBuffer commandBuffer, Buffer& dstBuffer, VkDeviceSize srcOffset, VkDeviceSize dstOffset, VkDeviceSize copySize);

private:
    std::shared_ptr<VulkanInfo> m_vkInfo;

};

