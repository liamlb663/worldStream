// src/RenderEngine/RenderObjects/PipelineBuilder.cpp

#include "DescriptorLayoutBuilder.hpp"

#include "RenderEngine/VkUtils.hpp"

DescriptorLayoutBuilder* DescriptorLayoutBuilder::addBinding(
        U32 binding,
        VkDescriptorType type,
        VkShaderStageFlags stages
) {
    VkDescriptorSetLayoutBinding bindingInfo = {
        .binding = binding,
        .descriptorType = type,
        .descriptorCount = 1,
        .stageFlags = stages,
        .pImmutableSamplers = nullptr,
    };

    m_bindings.push_back(bindingInfo);

    return this;
}

VkDescriptorSetLayout DescriptorLayoutBuilder::build(VkDevice device) {
    VkDescriptorSetLayoutCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .bindingCount = static_cast<U32>(m_bindings.size()),
        .pBindings = m_bindings.data()

    };

    VkDescriptorSetLayout output;
    VkResult result = vkCreateDescriptorSetLayout(
            device,
            &info,
            nullptr,
            &output
    );
    if (!VkUtils::checkVkResult(result, "Error creating descriptor set layout!")) {
        return nullptr;
    }

    return output;
}

