// src/RenderEngine/RenderObjects/PipelineBuilder.cpp

#include "DescriptorLayoutBuilder.hpp"

#include "RenderEngine/VkUtils.hpp"

DescriptorLayoutBuilder* DescriptorLayoutBuilder::addBinding(
        U32 bindingNumber,
        VkDescriptorType type,
        VkShaderStageFlags stages,
        U32 size
) {
    VkDescriptorSetLayoutBinding binding = {
        .binding = bindingNumber,
        .descriptorType = type,
        .descriptorCount = 1,
        .stageFlags = stages,
        .pImmutableSamplers = nullptr,
    };

    DescriptorBindingInfo bindingInfo = {
        .binding = bindingNumber,
        .descriptorType = type,
        .stages = stages,
        .size = size
    };

    m_bindings.push_back(binding);
    m_bindingInfos.push_back(bindingInfo);

    return this;
}

Option<DescriptorLayoutInfo> DescriptorLayoutBuilder::build(VkDevice device) {
    VkDescriptorSetLayoutCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_DESCRIPTOR_BUFFER_BIT_EXT,
        .bindingCount = static_cast<U32>(m_bindings.size()),
        .pBindings = m_bindings.data()

    };

    VkDescriptorSetLayout layout;
    VkResult result = vkCreateDescriptorSetLayout(
            device,
            &info,
            nullptr,
            &layout
    );
    if (!VkUtils::checkVkResult(result, "Error creating descriptor set layout!")) {
        return std::nullopt;
    }

    DescriptorLayoutInfo output = {
        .layout = layout,
        .bindings = m_bindingInfos
    };

    return output;
}

void DescriptorLayoutBuilder::clear() { 
    m_bindings.clear(); 
};
