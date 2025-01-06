// src/ResourceManagement/RenderResources/SparseBuffer.cpp

#include "SparseBuffer.hpp"

#include "RenderEngine/VkUtils.hpp"

#include <spdlog/spdlog.h>

bool SparseBuffer::init(
        std::shared_ptr<VulkanInfo> vkInfo,
        Size size,
        VkBufferUsageFlags bufferUsage,
        VmaMemoryUsage memoryUsage,
        VmaAllocationCreateFlags allocFlags
) {
    m_vkInfo = vkInfo;
    m_allocFlags = allocFlags;
    m_memoryUsage = memoryUsage;

    // Query memory granularity // TODO: Use CVAR system to retrieve this later
    VkPhysicalDeviceProperties2 deviceProperties = {};
    deviceProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    vkGetPhysicalDeviceProperties2(m_vkInfo->physicalDevice, &deviceProperties);
    m_granularity = deviceProperties.properties.limits.bufferImageGranularity;

    return buffer.init(
        vkInfo,
        size,
        bufferUsage |
            VK_BUFFER_CREATE_SPARSE_BINDING_BIT |
            VK_BUFFER_CREATE_SPARSE_RESIDENCY_BIT,
        memoryUsage,
        allocFlags
    );
};

void SparseBuffer::shutdown() {
    m_pendingBinds.clear();
    buffer.shutdown();
}

void SparseBuffer::bindMemory(Size size, Size offset) {
    Allocation* allocation = allocateMemory(size, offset);

    VkSparseMemoryBind bind = {
        .resourceOffset = offset,
        .size = size,
        .memory = allocation->info.deviceMemory,
        .memoryOffset = allocation->info.offset,
        .flags = 0,
    };

    m_pendingBinds.push_back(bind);
}

void SparseBuffer::unbindMemory(Size size, Size offset) {
    // Find allocation
    Allocation allocation = {{},{},0,0};
    for (Size i = 0; i < m_allocations.size(); i++) {
        Allocation candidate = m_allocations[i];
        if (candidate.size == size && candidate.offset == offset) {
            allocation = candidate;
        }
    }

    if (allocation.size != size || allocation.offset != offset) {
        spdlog::warn("Allocation not found for unbinding at offset {} with size {}!", offset, size);
        return;
    }

    VkSparseMemoryBind bind = {
        .resourceOffset = offset,
        .size = size,
        .memory = VK_NULL_HANDLE, // Null memory unbinds the range
        .memoryOffset = 0,
        .flags = 0
    };

    m_pendingBinds.push_back(bind);
    freeMemory(allocation);
}

void SparseBuffer::flushPendingBinds() {
    if (m_pendingBinds.empty()) return;

    VkBindSparseInfo bindInfo = {};
    bindInfo.sType = VK_STRUCTURE_TYPE_BIND_SPARSE_INFO;
    bindInfo.bufferBindCount = 1;

    VkSparseBufferMemoryBindInfo bufferBindInfo = {};
    bufferBindInfo.buffer = buffer.buffer;
    bufferBindInfo.bindCount = static_cast<uint32_t>(m_pendingBinds.size());
    bufferBindInfo.pBinds = m_pendingBinds.data();

    bindInfo.pBufferBinds = &bufferBindInfo;

    VkResult result = vkQueueBindSparse(m_vkInfo->transferQueue, 1, &bindInfo, VK_NULL_HANDLE);
    if (!VkUtils::checkVkResult(result, "Failed to bind sparse memory")) {
        return;
    }

    m_pendingBinds.clear();
}

SparseBuffer::Allocation* SparseBuffer::allocateMemory(Size size, Size offset) {
    VmaAllocationCreateInfo allocInfo = {
        .flags = m_allocFlags,
        .usage = m_memoryUsage,
        .requiredFlags = 0,
        .preferredFlags = 0,
        .memoryTypeBits = 0,
        .pool = nullptr,
        .pUserData = nullptr,
        .priority = 0,
    };

    VmaAllocation vmaAllocation;
    VkResult res = vmaAllocateMemoryForBuffer(m_vkInfo->allocator, buffer.buffer, &allocInfo, &vmaAllocation, nullptr);
    if (!VkUtils::checkVkResult(res, "Failed to allocate sparse memory!")) {
        return nullptr;
    }

    VmaAllocationInfo info;
    vmaGetAllocationInfo(m_vkInfo->allocator, vmaAllocation, &info); // Get memory properties

    Allocation allocation = {
        .allocation = vmaAllocation,
        .info = info,
        .size = size,
        .offset = offset,
    };

    m_allocations.push_back(allocation);
    return &m_allocations[m_allocations.size() - 1];
}

void SparseBuffer::freeMemory(Allocation allocation) {
    vmaFreeMemory(m_vkInfo->allocator, allocation.allocation);

    // Remove from allocated list
    for (Size i = 0; i < m_allocations.size(); i++) {
        const Allocation& candidate = m_allocations[i];
        if (candidate.size == allocation.size && candidate.offset == allocation.offset) {
            m_allocations.erase(m_allocations.begin() + i);
            return;
        }
    }

    spdlog::warn("Failed to find and remove Allocation from tracked state.");
}

Size SparseBuffer::alignSize(Size size) {
    return (size + m_granularity - 1) & ~(m_granularity - 1);
}

void* SparseBuffer::mapMemory(Size offset, Size size) {
    for (const Allocation& allocation : m_allocations) {
        if (allocation.offset == offset && allocation.size >= size) {
            void* data = nullptr;
            VkResult result = vmaMapMemory(m_vkInfo->allocator, allocation.allocation, &data);
            if (!VkUtils::checkVkResult(result, "Failed to map memory!")) {
                return nullptr;
            }

            return static_cast<char*>(data) + (offset - allocation.offset);
        }
    }

    spdlog::warn("No allocation found for mapping the specified region");
    return nullptr;
}

void SparseBuffer::unmapMemory(Size offset) {
    // Find the allocation for the region
    for (const Allocation& allocation : m_allocations) {
        if (allocation.offset <= offset && allocation.offset + allocation.size > offset) {
            // Unmap the memory
            vmaUnmapMemory(m_vkInfo->allocator, allocation.allocation);
            return;
        }
    }

    // If no matching allocation is found, log a warning
    spdlog::warn("No allocation found for unmapping the specified region");
}

void SparseBuffer::updateData(const void* data, Size size, Size offset) {
    void* mappedMemory = mapMemory(offset, size);
    std::memcpy(mappedMemory, data, size);
    unmapMemory(offset);
}
