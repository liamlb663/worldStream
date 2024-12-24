// src/RenderEngine/RenderResources/Fence.hpp

#pragma once

#include "../VulkanInfo.hpp"

#include <cstdint>
#include <memory>
#include <vulkan/vulkan.h>

class Fence {
public:
    bool initialize(std::shared_ptr<VulkanInfo> vkInfo, bool signaled, std::string name);
    void shutdown();

    bool wait(uint64_t timeout = UINT64_MAX);
    bool reset();

    VkFence get() const;

private:
    VkFence m_fence = VK_NULL_HANDLE;
    std::shared_ptr<VulkanInfo> m_vkInfo;
};

