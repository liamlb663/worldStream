// src/ResourceManagement/RenderResources/Fence.hpp

#pragma once

#include "RenderEngine/VulkanInfo.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <memory>

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

