// src/ResourceManagement/RenderResources/RenderObjects/Materials.hpp

#pragma once

#include "../DescriptorBuffer.hpp"

#include <vulkan/vulkan.h>

#include <memory>
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
    std::shared_ptr<DescriptorBuffer> buffer;
    U32 descriptorIndex;
};

struct MaterialData {
    std::shared_ptr<MaterialInfo> pipeline;
    std::vector<DescriptorInfo> descriptors;

};

