// src/RenderEngine/RenderObjects/PipelineBuilder.hpp

#pragma once

#include "Core/Types.hpp"
#include "Materials.hpp"

#include <vector>
#include <vulkan/vulkan.h>

class DescriptorLayoutBuilder {
public:
    DescriptorLayoutBuilder* addBinding(
            U32 binding,
            VkDescriptorType type,
            VkShaderStageFlags stages,
            U32 size
    );

    Option<DescriptorLayoutInfo> build(VkDevice device);

    void clear();

private:
    std::vector<VkDescriptorSetLayoutBinding> m_bindings;
    std::vector<DescriptorBindingInfo> m_bindingInfos;
};

