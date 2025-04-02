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
    U32 alignment;
    // U32 stride;  // TODO: Determine if nessesary, should be in storage

    bool operator==(const DescriptorBindingInfo &b) const {
        return binding == b.binding &&
            descriptorType == b.descriptorType &&
            stages == b.stages &&
            size == b.size &&
            alignment == b.alignment;
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

struct DescriptorInfo {
    DescriptorBuffer* buffer = nullptr;
    U32 descriptorIndex = -1;
};

struct MaterialData {
    MaterialInfo* pipeline;
    std::vector<DescriptorInfo> descriptors;
};

