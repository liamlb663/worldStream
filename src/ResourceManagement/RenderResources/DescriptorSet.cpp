// src/ResourceManagement/RenderResources/DescriptorSet.cpp

#include "DescriptorSet.hpp"

#include <spdlog/spdlog.h>

#include <cstring>
#include <numeric>

bool DescriptorSet::init(std::shared_ptr<VulkanInfo> vkInfo, Size size) {
    m_vkInfo = vkInfo;

    VkBufferUsageFlags bufferUsage =
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT |
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
        VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

    VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    VmaAllocationCreateFlags allocFlags =
        VMA_ALLOCATION_CREATE_MAPPED_BIT;

    bool bufferReturn = m_buffer.init(
            vkInfo,
            size,
            bufferUsage,
            memoryUsage,
            allocFlags
    );

    if (!bufferReturn) {
        return false;
    }

    m_address = getDeviceAddress();

    return true;
};

void DescriptorSet::shutdown() {
    m_buffer.shutdown();
}

void DescriptorSet::addBinding(Size size) {
    Size sum = std::accumulate(m_bindings.begin(), m_bindings.end(), size);
    if (sum > getSize()) {
        spdlog::error("Added Binding too big for buffer!");
    }

    m_bindings.push_back(size);
}

std::vector<VkDescriptorBufferBindingInfoEXT> DescriptorSet::getBindingInfo() const {
    std::vector<VkDescriptorBufferBindingInfoEXT> output;

    VkBufferUsageFlags usage =
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT |
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

    Size offset = 0;
    for (Size i = 0; i < m_bindings.size(); i++){
        VkDescriptorBufferBindingInfoEXT bindingInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_BUFFER_BINDING_INFO_EXT,
            .pNext = nullptr,
            .address = m_address + offset,
            .usage = usage,
        };

        output.push_back(bindingInfo);
        offset += m_bindings[i];
    }

    return output;
}

Size DescriptorSet::getSize() const {
    return m_buffer.info.size;
}

void DescriptorSet::update(void* data, Size size, Size offset) {
    if (size + offset > m_buffer.info.size) {
        spdlog::error("DescriptorBuffer update exceeds buffer size!");
        return;
    }

    m_buffer.map();

    memcpy(static_cast<U8*>(m_buffer.info.pMappedData) + offset, data, size);

    m_buffer.unmap();
}

VkDeviceAddress DescriptorSet::getDeviceAddress() const {
    VkBufferDeviceAddressInfo addressInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
        .pNext = nullptr,
        .buffer = m_buffer.buffer,
    };

    return vkGetBufferDeviceAddress(m_vkInfo->device, &addressInfo);
}

