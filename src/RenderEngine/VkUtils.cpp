// src/RenderEngine/VkUtils.hpp

#include "VkUtils.hpp"

#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <vulkan/vk_enum_string_helper.h>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

bool VkUtils::checkVkResult(VkResult result, std::string ErrorMessage) {
    if (result != VK_SUCCESS) {
        spdlog::error(fmt::format("{}: {}", ErrorMessage, string_VkResult(result)));
        return false;
    }

    return true;
}

U32 VkUtils::findMemoryType(VulkanInfo* vkInfo, U32 typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(vkInfo->physicalDevice, &memProperties);

    for (U32 i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    spdlog::error("Failed to find suitable memory type!");
    return -1;
}

