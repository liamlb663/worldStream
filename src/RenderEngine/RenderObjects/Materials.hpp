// src/RenderEngine/RenderObjects/Materials.cpp

#pragma once

#include "ResourceManagement/RenderResources/DescriptorBuffer.hpp"
#include <vulkan/vulkan.h>

#include <vector>

enum MaterialType {
    Opaque,
    Transparent
};

struct MaterialInfo {
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;
    std::vector<VkDescriptorSetLayout> descriptorLayouts;
    MaterialType type;
};

struct DescriptorInfo {
    DescriptorBuffer* buffer;
    U32 descriptorIndex;
};

struct MaterialData {
    MaterialInfo* pipeline;
    std::vector<DescriptorInfo> descriptors;
};

