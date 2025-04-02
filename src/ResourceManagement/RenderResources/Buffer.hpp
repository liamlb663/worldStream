// src/ResourceManagement/RenderResources/Buffer.hpp

#pragma once

#include "RenderEngine/VulkanInfo.hpp"
#include "spdlog/spdlog.h"

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

Buffer() {
    spdlog::debug("Buffer DEFAULT constructor: {}", static_cast<void*>(this));
}

Buffer(const Buffer& other) {
    spdlog::debug("Buffer COPY constructor: this={}, other={}, other.inited={}, other.m_vkInfo={}",
                  static_cast<void*>(this), static_cast<const void*>(&other),
                  other.inited, static_cast<void*>(other.m_vkInfo.get()));
    *this = other;
}
Buffer(Buffer&& other) noexcept {
    spdlog::debug("Buffer MOVE constructor: this={}, other={}", static_cast<void*>(this), static_cast<const void*>(&other));
    *this = std::move(other);
}
Buffer& operator=(const Buffer& other) {
    spdlog::debug("Buffer COPY assignment: this={}, other={}", static_cast<void*>(this), static_cast<const void*>(&other));
    buffer = other.buffer;
    allocation = other.allocation;
    info = other.info;
    address = other.address;
    m_vkInfo = other.m_vkInfo;
    inited = other.inited;
    return *this;
}
Buffer& operator=(Buffer&& other) noexcept {
    spdlog::debug("Buffer MOVE assignment: this={}, other={}", static_cast<void*>(this), static_cast<const void*>(&other));

    // Save source info before we move it
    auto* originalVkInfo = other.m_vkInfo.get();
    auto originalInited = other.inited;

    // Perform the move
    buffer = other.buffer;
    allocation = other.allocation;
    info = other.info;
    address = other.address;
    m_vkInfo = std::move(other.m_vkInfo);
    inited = other.inited;

    // Invalidate other
    other.buffer = VK_NULL_HANDLE;
    other.allocation = VK_NULL_HANDLE;
    other.info = {};
    other.address = 0;
    other.inited = 0;
    other.m_vkInfo = nullptr;

    // 🚨 Runtime check for a *bad move*
    if (originalInited == 1 && originalVkInfo != nullptr && m_vkInfo == nullptr) {
        spdlog::critical("!!! BAD MOVE DETECTED: valid buffer moved into invalid state");
        spdlog::critical("   this={} (post-move), other={} (moved-from)", static_cast<void*>(this), static_cast<void*>(&other));
        spdlog::critical("   originalVkInfo = {}, now m_vkInfo = nullptr", static_cast<void*>(originalVkInfo));
        std::abort(); // 💥 CRASH exactly where the move *goes wrong*
    }

    return *this;
}

private:
    std::shared_ptr<VulkanInfo> m_vkInfo;

    U32 inited = 0;
};

