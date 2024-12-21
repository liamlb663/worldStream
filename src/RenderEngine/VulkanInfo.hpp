// src/RenderEngine/RenderEngine.hpp

#pragma once

#include "Core/Types.hpp"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

class CommandPool;  // Forward Declaration

typedef struct VulkanInfo {
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;

    VkDevice device;
    VkPhysicalDevice physicalDevice;

    VkQueue graphicsQueue;
    U32 graphicsQueueFamily;

    VkQueue transferQueue;
    U32 transferQueueFamily;

    VmaAllocator allocator;
    CommandPool* transferPool;
} VulkanInfo;

