// src/RenderEngine/RenderObjects/Materials.hpp

#pragma once

#include "Core/Types.hpp"
#include <vulkan/vulkan.h>
class DescriptorBuffer;

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
    U32 offset;

    bool operator==(const DescriptorBindingInfo &b) const {
        return binding == b.binding &&
            descriptorType == b.descriptorType &&
            stages == b.stages &&
            size == b.size &&
            offset == b.offset;
    }
};

struct DescriptorSetInfo {
    VkDescriptorSetLayout layout;
    std::vector<DescriptorBindingInfo> bindings;

    bool operator==(const DescriptorSetInfo &b) const {
        return layout == b.layout &&
            bindings == b.bindings;
    }
};

struct MaterialInfo {
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;
    std::vector<DescriptorSetInfo> descriptorSets;
    MaterialType type;
};

struct DescriptorSetData {
    DescriptorBuffer* buffer;
    U32 set;
    U32 descriptorIndex;
    std::vector<U32> bindings;
};

struct MaterialData {
    MaterialInfo* pipeline;
    std::vector<DescriptorSetData> descriptorSets;
};

