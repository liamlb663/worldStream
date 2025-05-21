// src/RenderEngine/VkUtils.hpp

#pragma once

#include "Core/Types.hpp"
#include "RenderEngine/VulkanInfo.hpp"
#include <vulkan/vulkan.h>

#include <string>

namespace VkUtils {
    bool checkVkResult(VkResult result, std::string ErrorMessage);
    U32 findMemoryType(VulkanInfo* vkInfo, U32 typeFilter, VkMemoryPropertyFlags properties);
}
