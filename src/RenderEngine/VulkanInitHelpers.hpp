// src/RenderEngine/VulkanInitHelpers.hpp
#pragma once

#include "Core/Types.hpp"
#include <vulkan/vulkan.h>

bool CreateVulkanInstance(
    VkInstance* instance,
    bool enableValidationLayers
);

bool PickPhysicalDevice(
    VkInstance instance,
    VkSurfaceKHR surface,
    VkPhysicalDevice* physicalDevice,
    U32* graphicsFamily,
    U32* transferFamily
);

bool CreateLogicalDevice(
    VkPhysicalDevice physicalDevice,
    VkDevice* device, VkQueue* graphicsQueue, VkQueue* transferQueue,
    U32 graphicsFamily, U32 transferFamily,
    VkPhysicalDeviceFeatures& features10,
    VkPhysicalDeviceVulkan12Features& features12,
    VkPhysicalDeviceVulkan13Features& features13,
    VkPhysicalDeviceDescriptorBufferFeaturesEXT& descriptorBufferFeatures
);

bool SetupDebugMessenger(
    VkInstance instance,
    VkDebugUtilsMessengerEXT* debugMessenger
);

void DestroyDebugMessenger(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger
);
