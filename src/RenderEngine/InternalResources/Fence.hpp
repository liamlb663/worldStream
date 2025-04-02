// src/RenderEngine/InternalResources/Fence.hpp

#pragma once

#include "../VulkanInfo.hpp"

#include <cstdint>
#include <vulkan/vulkan.h>
#include <string>

class Fence {
public:
    bool initialize(VulkanInfo* vkInfo, bool signaled, std::string name);
    void shutdown();

    bool wait(uint64_t timeout = UINT64_MAX);
    bool reset();

    VkFence get() const;

private:
    VkFence m_fence = VK_NULL_HANDLE;
    VulkanInfo* m_vkInfo;
};

