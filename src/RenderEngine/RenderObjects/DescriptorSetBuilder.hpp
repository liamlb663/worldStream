// src/RenderEngine/RenderObjects/DescriptorSetBuilder.hpp

#pragma once

#include "Core/Types.hpp"
#include "Materials.hpp"

#include <vector>
#include <vulkan/vulkan.h>

class DescriptorSetBuilder {
public:
    DescriptorSetBuilder* addBinding(
            U32 binding,
            VkDescriptorType type,
            VkShaderStageFlags stages,
            U32 size,
            U32 offset
    );

    Option<DescriptorSetInfo> build(VkDevice device);

    void clear();

private:
    std::vector<VkDescriptorSetLayoutBinding> m_bindings;
    std::vector<DescriptorBindingInfo> m_bindingInfos;
};

