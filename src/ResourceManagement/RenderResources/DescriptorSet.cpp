// src/ResourceManagement/RenderResources/DescriptorSet.cpp

#include "DescriptorSet.hpp"

#include <spdlog/spdlog.h>
#include <vulkan/vk_enum_string_helper.h>

bool DescriptorSet::init(VulkanInfo* vkInfo, VkDescriptorSet set) {
    m_vkInfo = vkInfo;
    m_descriptorSet = set;
    return true;
}

void DescriptorSet::writeUniformBuffer(U32 binding, Buffer* buffer, VkDeviceSize size, VkDeviceSize offset) {
    VkDescriptorBufferInfo bufferInfo = {
        .buffer = buffer->buffer,
        .offset = offset,
        .range = size,
    };

    VkWriteDescriptorSet write = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = m_descriptorSet,
        .dstBinding = binding,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pImageInfo = nullptr,
        .pBufferInfo = &bufferInfo,
        .pTexelBufferView = nullptr,
    };

    vkUpdateDescriptorSets(m_vkInfo->device, 1, &write, 0, nullptr);
}

void DescriptorSet::writeImageSampler(U32 binding, Image* image, Sampler sampler) {
    VkDescriptorImageInfo imageInfo = {
        .sampler = sampler.get(),
        .imageView = image->view,
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };

    VkWriteDescriptorSet write = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = m_descriptorSet,
        .dstBinding = binding,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo = &imageInfo,
        .pBufferInfo = nullptr,
        .pTexelBufferView = nullptr,
    };

    vkUpdateDescriptorSets(m_vkInfo->device, 1, &write, 0, nullptr);
}

void DescriptorSet::bindBuffer(
    VkCommandBuffer commandBuffer,
    VkPipelineBindPoint pipelineBindPoint,
    VkPipelineLayout pipelineLayout,
    U32 setIndex
) {
    vkCmdBindDescriptorSets(
        commandBuffer,
        pipelineBindPoint,
        pipelineLayout,
        setIndex,
        1,
        &m_descriptorSet,
        0,
        nullptr
    );
};

