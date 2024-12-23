// src/RenderEngine/Config.hpp

#pragma once

#include "Core/Types.hpp"
#include <vulkan/vulkan.h>

namespace Config {
    constexpr bool useValidationLayers = true;
    constexpr Size framesInFlight = 3;

    constexpr VkFormat swapchainFormat = VK_FORMAT_B8G8R8A8_UNORM;
    constexpr VkColorSpaceKHR swapchainColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    constexpr VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;

    constexpr VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;
    constexpr VkFormat drawFormat = VK_FORMAT_R16G16B16A16_SFLOAT;

}

