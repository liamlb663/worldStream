// src/RenderEngine/RenderObjects/DescriptorSetBuilder.cpp

#include "DescriptorSetBuilder.hpp"

#include "RenderEngine/VkUtils.hpp"

DescriptorSetBuilder* DescriptorSetBuilder::addBinding(
        U32 bindingNumber,
        VkDescriptorType type,
        VkShaderStageFlags stages,
        U32 size,
        U32 offset
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
        .size = size,
        .offset = offset,
    };

    m_bindings.push_back(binding);
    m_bindingInfos.push_back(bindingInfo);

    return this;
}

Option<DescriptorSetInfo> DescriptorSetBuilder::build(VkDevice device) {
    VkDescriptorSetLayoutCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
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

    DescriptorSetInfo output = {
        .layout = layout,
        .bindings = m_bindingInfos,
    };

    return output;
}

void DescriptorSetBuilder::clear() { 
    m_bindings.clear(); 
};
