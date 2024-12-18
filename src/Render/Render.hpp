// src/Render/Render.hpp

#pragma once

#include <vulkan/vulkan.h>

typedef struct VulkanInfo {
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;

    VkDevice device;
    VkPhysicalDevice physicalDevice;

} VulkanInfo;

class Render {
public:
    bool initialize();

    void shutdown();

private:
    bool initVulkan();

    VulkanInfo m_vkInfo;
};

