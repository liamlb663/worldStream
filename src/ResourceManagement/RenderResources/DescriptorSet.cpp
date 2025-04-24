// src/ResourceManagement/RenderResources/DescriptorSet.cpp

#include "DescriptorSet.hpp"

#include <spdlog/spdlog.h>

bool DescriptorSet::init(VulkanInfo* vkInfo, VkDescriptorSet set) {
    m_vkInfo = vkInfo;
    m_descriptorSet = set;
    return true;
}

void DescriptorSet::writeUniformBuffer(U32 binding, Buffer* buffer, VkDeviceSize size, VkDeviceSize offset) {
    m_writeEntries.emplace_back();
    WriteEntry& entry = m_writeEntries.back();

    entry.bufferInfo = {
        .buffer = buffer->buffer,
        .offset = offset,
        .range = size,
    };

    entry.write = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = m_descriptorSet,
        .dstBinding = binding,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pImageInfo = nullptr,
        .pBufferInfo = &entry.bufferInfo,
        .pTexelBufferView = nullptr,
    };
}

void DescriptorSet::writeImageSampler(U32 binding, Image* image, Sampler sampler) {
    m_writeEntries.emplace_back();
    WriteEntry& entry = m_writeEntries.back();

    entry.imageInfo = {
        .sampler = sampler.get(),
        .imageView = image->view,
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };

    entry.write = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = m_descriptorSet,
        .dstBinding = binding,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo = &entry.imageInfo,
        .pBufferInfo = nullptr,
        .pTexelBufferView = nullptr,
    };
}

void DescriptorSet::update() {
    if (m_writeEntries.empty())
        return;

    std::vector<VkWriteDescriptorSet> writes;
    writes.reserve(m_writeEntries.size());

    for (WriteEntry& entry : m_writeEntries)
        writes.push_back(entry.write);

    vkUpdateDescriptorSets(
        m_vkInfo->device,
        static_cast<U32>(writes.size()),
        writes.data(),
        0,
        nullptr
    );

    m_writeEntries.clear();
}

