// src/ResourceManagement/RenderResources/SparseBuffer.hpp

#pragma once

#include "Buffer.hpp"
#include "Core/Types.hpp"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <vector>
#include <memory>

class SparseBuffer {
public:
    Buffer buffer;

    bool init(
            std::shared_ptr<VulkanInfo> vkInfo,
            Size size,
            VkBufferUsageFlags bufferUsage,
            VmaMemoryUsage memoryUsage,
            VmaAllocationCreateFlags allocFlags,
            Size pageSize
    );
    void shutdown();

    void bindMemory(Size size, Size offset);
    void unbindMemory(Size size, Size offset);
    void flushPendingBinds();

    void updateData(const void* data, Size size, Size offset);

    Size getPageSize() const;

private:
    struct Allocation {
        VmaAllocation allocation;
        VmaAllocationInfo info;
        Size size;
        Size offset;
    };

    Allocation* allocateMemory(Size size, Size offset);
    void freeMemory(Allocation allocation);

    Size align(Size size);

    void* mapMemory(Size offset, Size size);
    void unmapMemory(Size offset);

    std::shared_ptr<VulkanInfo> m_vkInfo;
    Size m_pageSize;
    VmaMemoryUsage m_memoryUsage;
    VmaAllocationCreateFlags m_allocFlags;

    std::vector<VkSparseMemoryBind> m_pendingBinds;
    std::vector<Allocation> m_allocations;

};

