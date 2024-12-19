// src/RenderEngine/VkUtils.hpp

#pragma once

#include <vulkan/vulkan.h>

#include <string>

namespace VkUtils {
    bool checkVkResult(VkResult result, std::string ErrorMessage);

}
