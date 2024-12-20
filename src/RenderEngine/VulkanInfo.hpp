// src/RenderEngine/RenderEngine.hpp

#pragma once

#include "Core/Types.hpp"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

typedef struct VulkanInfo {
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;

    VkDevice device;
    VkPhysicalDevice physicalDevice;

    VkQueue graphicsQueue;
    U32 graphicsQueueFamily;

    VmaAllocator allocator;
} VulkanInfo;

