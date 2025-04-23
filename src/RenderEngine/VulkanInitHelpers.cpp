// src/RenderEngine/VulkanInitHelpers.hpp

#include "VulkanInitHelpers.hpp"
#include "GLFW/glfw3.h"
#include "RenderEngine/Debug.hpp"
#include "RenderEngine/VkUtils.hpp"
#include "spdlog/spdlog.h"
#include <vector>

bool CreateVulkanInstance(VkInstance* instance, bool enableValidationLayers) {
    if (!glfwInit()) {
        spdlog::error("Failed to initialize GLFW");
        return false;
    }

    // Query extensions from GLFW
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> instanceExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "World Streaming Engine",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "Custom Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_4,
    };

    U32 enabledLayerCount = 0;
    const char** validationLayersPtr = nullptr;
    if (enableValidationLayers) {
        enabledLayerCount = static_cast<U32>(validationLayers.size());
        validationLayersPtr = validationLayers.data();
    }

    VkInstanceCreateInfo instanceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = enabledLayerCount,
        .ppEnabledLayerNames = validationLayersPtr,
        .enabledExtensionCount = static_cast<U32>(instanceExtensions.size()),
        .ppEnabledExtensionNames = instanceExtensions.data(),
    };

    VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, instance);
    if (!VkUtils::checkVkResult(result, "Error creating Instance")) {
        return false;
    }

    return true;
}

bool PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface, VkPhysicalDevice* physicalDevice, U32* graphicsFamily, U32* transferFamily) {
    U32 deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) return false;

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (const auto& device : devices) {
        U32 queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int graphicsIndex = -1;
        int transferIndex = -1;

        for (U32 i = 0; i < queueFamilyCount; ++i) {
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

            if ((queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && presentSupport && graphicsIndex == -1) {
                graphicsIndex = i;
            }

            if ((queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) && !(queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && transferIndex == -1) {
                transferIndex = i;
            }
        }

        if (graphicsIndex != -1) {
            *graphicsFamily = graphicsIndex;
            *transferFamily = (transferIndex != -1) ? transferIndex : graphicsIndex;
            *physicalDevice = device;
            return true;
        }
    }
    return false;
}

bool CreateLogicalDevice(VkPhysicalDevice physicalDevice,
                         VkDevice* device, VkQueue* graphicsQueue, VkQueue* transferQueue,
                         U32 graphicsFamily, U32 transferFamily,
                         VkPhysicalDeviceFeatures& features10,
                         VkPhysicalDeviceVulkan12Features& features12,
                         VkPhysicalDeviceVulkan13Features& features13,
                         VkPhysicalDeviceDescriptorBufferFeaturesEXT& descriptorBufferFeatures) {
    descriptorBufferFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_FEATURES_EXT;
    descriptorBufferFeatures.descriptorBuffer = VK_TRUE;

    features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    features13.dynamicRendering = VK_TRUE;
    features13.synchronization2 = VK_TRUE;
    features13.pNext = &descriptorBufferFeatures;

    features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    features12.bufferDeviceAddress = VK_TRUE;
    features12.descriptorIndexing = VK_TRUE;
    features12.pNext = &features13;

    features10.samplerAnisotropy = VK_TRUE;
    features10.shaderSampledImageArrayDynamicIndexing = VK_TRUE;
    features10.sparseBinding = VK_TRUE;
    features10.sparseResidencyBuffer = VK_TRUE;

    std::vector<const char*> extensions = {
        "VK_KHR_swapchain",
    };

    float queuePriority = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    VkDeviceQueueCreateInfo graphicsQueueCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = graphicsFamily,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority,
    };
    queueCreateInfos.push_back(graphicsQueueCreateInfo);

    if (transferFamily != graphicsFamily) {
        VkDeviceQueueCreateInfo transferQueueCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = transferFamily,
            .queueCount = 1,
            .pQueuePriorities = &queuePriority,
        };
        queueCreateInfos.push_back(transferQueueCreateInfo);
    }

    VkDeviceCreateInfo deviceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &features12,
        .flags = 0,
        .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
        .pEnabledFeatures = &features10,
    };

    VkResult res = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, device);
    if (!VkUtils::checkVkResult(res, "Failed to create device"))
        return false;

    vkGetDeviceQueue(*device, graphicsFamily, 0, graphicsQueue);
    vkGetDeviceQueue(*device, transferFamily, 0, transferQueue);

    return true;
}

bool SetupDebugMessenger(VkInstance instance, VkDebugUtilsMessengerEXT* debugMessenger) {
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = Debug::CustomDebugCallback;
    createInfo.pUserData = nullptr;

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        VkResult result = func(instance, &createInfo, nullptr, debugMessenger);
        return result == VK_SUCCESS;
    } else {
        spdlog::error("Could not load vkCreateDebugUtilsMessengerEXT.");
        return false;
    }
}

void DestroyDebugMessenger(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, nullptr);
    }
}
