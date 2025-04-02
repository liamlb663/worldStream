// src/RenderEngine/InternalResources/Semaphore.hpp

#pragma once

#include "../VulkanInfo.hpp"

#include <string>
#include <vulkan/vulkan.h>

class Semaphore {
public:
    bool initialize(VulkanInfo* vkInfo, std::string name);
    void shutdown();

    VkSemaphore get() const;

private:
    VkSemaphore m_semaphore = VK_NULL_HANDLE;
    VulkanInfo* m_vkInfo;

};

