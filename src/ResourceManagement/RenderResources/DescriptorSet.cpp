// src/ResourceManagement/RenderResources/DescriptorSet.cpp

#include "DescriptorSet.hpp"
#include <spdlog/spdlog.h>

bool DescriptorSet::init(VulkanInfo* vkInfo, DescriptorPool* pool, const DescriptorSetInfo& setInfo) {
    m_vkInfo = vkInfo;
    m_pool = pool;
    m_setInfo = setInfo;

    m_descriptorSet = m_pool->allocate(setInfo.layout);
    if (m_descriptorSet == VK_NULL_HANDLE) {
        spdlog::error("Failed to allocate descriptor set.");
        return false;
    }

    return true;
}

void DescriptorSet::writeUniformBuffer(U32 binding, Buffer* buffer, VkDeviceSize size, VkDeviceSize offset) {
    VkDescriptorBufferInfo bufferInfo = {
        .buffer = buffer->buffer,
        .offset = offset,
        .range = size,
    };

    m_bufferInfos.push_back(bufferInfo);

    VkWriteDescriptorSet write = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = m_descriptorSet,
        .dstBinding = binding,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pImageInfo = nullptr,
        .pBufferInfo = &m_bufferInfos.back(),
        .pTexelBufferView = nullptr,
    };

    m_writes.push_back(write);
}

void DescriptorSet::writeImageSampler(U32 binding, Image* image, VkSampler sampler) {
    VkDescriptorImageInfo imageInfo = {
        .sampler = sampler,
        .imageView = image->view,
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };

    m_imageInfos.push_back(imageInfo);

    VkWriteDescriptorSet write = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = m_descriptorSet,
        .dstBinding = binding,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo = &m_imageInfos.back(),
        .pBufferInfo = nullptr,
        .pTexelBufferView = nullptr,
    };

    m_writes.push_back(write);
}

void DescriptorSet::update() {
    if (m_writes.empty())
        return;

    vkUpdateDescriptorSets(
        m_vkInfo->device,
        static_cast<U32>(m_writes.size()),
        m_writes.data(),
        0,
        nullptr
    );

    m_writes.clear();
    m_bufferInfos.clear();
    m_imageInfos.clear();
}

