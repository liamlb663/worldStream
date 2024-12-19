// src/RenderEngine/RenderEngine.hpp

#pragma once

#include "Core/DeletionQueue.hpp"
#include "Window.hpp"
#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"

typedef struct VulkanInfo {
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;

    VkDevice device;
    VkPhysicalDevice physicalDevice;

    VkQueue graphicsQueue;
    uint32_t graphicsQueueFamily;

    VmaAllocator allocator;
} VulkanInfo;

class RenderEngine {
public:
    bool initialize();

    void shutdown();

private:
    bool initVulkan();

    VulkanInfo m_vkInfo;
    Window* m_window;

    DeletionQueue m_mainDeletionQueue;
};

