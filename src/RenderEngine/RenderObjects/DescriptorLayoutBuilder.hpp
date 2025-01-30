// src/RenderEngine/RenderObjects/PipelineBuilder.hpp

#pragma once

#include "Core/Types.hpp"
#include <vector>
#include <vulkan/vulkan.h>

class DescriptorLayoutBuilder {
public:
    DescriptorLayoutBuilder* addBinding(
            U32 binding,
            VkDescriptorType type,
            VkShaderStageFlags stages
    );

    VkDescriptorSetLayout build(VkDevice device);

    void clear() { m_bindings.clear(); };
private:
    std::vector<VkDescriptorSetLayoutBinding> m_bindings;
};

