// src/RenderEngine/InternalResources/Semaphore.hpp

#pragma once

#include "../VulkanInfo.hpp"

#include <memory>
#include <vulkan/vulkan.h>

class Semaphore {
public:
    bool initialize(std::shared_ptr<VulkanInfo> vkInfo, std::string name);
    void shutdown();

    VkSemaphore get() const;

private:
    VkSemaphore m_semaphore = VK_NULL_HANDLE;
    std::shared_ptr<VulkanInfo> m_vkInfo;
};

