// src/RenderEngine/VkUtils.hpp

#include "VkUtils.hpp"

#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <vulkan/vk_enum_string_helper.h>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

bool VkUtils::checkVkResult(VkResult result, std::string ErrorMessage) {
    if (result != VK_SUCCESS) {
        spdlog::error(fmt::format("{}: {}", ErrorMessage, string_VkResult(result)));
        return false;
    }

    return true;
}
