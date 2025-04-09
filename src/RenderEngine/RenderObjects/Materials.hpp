// src/RenderEngine/RenderObjects/Materials.hpp

#pragma once

#include "ResourceManagement/RenderResources/DescriptorBuffer.hpp"
#include <vulkan/vulkan.h>

#include <vector>

enum MaterialType {
    Opaque,
    Transparent
};

struct DescriptorBindingInfo {
    U32 binding;
    VkDescriptorType descriptorType;
    VkShaderStageFlags stages;
    U32 size;

    bool operator==(const DescriptorBindingInfo &b) const {
        return binding == b.binding &&
            descriptorType == b.descriptorType &&
            stages == b.stages &&
            size == b.size;
    }
};

struct DescriptorLayoutInfo {
    VkDescriptorSetLayout layout;
    std::vector<DescriptorBindingInfo> bindings;

    bool operator==(const DescriptorLayoutInfo &b) const {
        return layout == b.layout &&
            bindings == b.bindings;
    }
};

struct MaterialInfo {
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;
    std::vector<DescriptorLayoutInfo> descriptorLayouts;
    MaterialType type;
};

struct DescriptorBindingData {
    U32 binding;
    U32 descriptorIndex;
};

struct DescriptorSetData {
    DescriptorBuffer* buffer;
    U32 set;
    std::vector<DescriptorBindingData> bindings;
};

struct MaterialData {
    MaterialInfo* pipeline;
    std::vector<DescriptorSetData> descriptorSets;
};

