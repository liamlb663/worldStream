// src/ResourceManagement/MaterialManager.hpp

#pragma once

#include <filesystem>
#include <vulkan/vulkan.h>

VkShaderModule LoadAndCompileShader(VkDevice device, const std::filesystem::path filename, VkShaderStageFlagBits vkStage);
