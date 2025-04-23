// src/RenderEngine/RenderObjects/Materials.hpp

#pragma once

#include "Core/Types.hpp"
#include "ResourceManagement/RenderResources/DescriptorSet.hpp"
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

struct PushConstantsInfo {
    bool enabled;
    VkShaderStageFlags stages;
    Size size;
    Size offset;
};

struct MaterialInfo {
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;
    PushConstantsInfo pushConstants;
    std::vector<DescriptorSetInfo> descriptorSets;
    MaterialType type;
};

struct DescriptorSetData {
    DescriptorSet set;
    U32 setIndex;
    std::vector<U32> bindings;
};

struct MaterialData {
    MaterialInfo* pipeline;
    std::vector<DescriptorSetData> descriptorSets;
    void* pushConstantData;
};

