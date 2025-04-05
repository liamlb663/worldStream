// src/ResourceManagement/MaterialManagerUtils/stringParsers.hpp

#pragma once

#include "RenderEngine/RenderObjects/PipelineBuilder.hpp"

#include <vulkan/vulkan.h>

#include <string>
#include <vector>

namespace MaterialManagerUtils {

VkDescriptorType getDescriptorType(std::string input);
VkShaderStageFlagBits getShaderStageFlagBit(std::string stage);
VkShaderStageFlags getShaderStageFlags(const std::vector<std::string>& stages);
BlendingMode getBlendingMode(std::string input);
VkFormat getFormat(std::string input);
VkSampleCountFlagBits getMultisampleCount(std::string input);
VkPolygonMode getPolygonMode(std::string input);
VkCullModeFlags getCullMode(std::string input);
VkFrontFace getFrontFace(std::string input);
VkPrimitiveTopology getTopology(std::string input);
VkCompareOp getCompareOp(std::string input);

}
