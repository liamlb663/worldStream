// src/RenderEngine/InternalResources/CommandPool.cpp

#include "CommandPool.hpp"

#include "../Debug.hpp"
#include "../VkUtils.hpp"

#include <memory>
#include <spdlog/spdlog.h>
#include <vulkan/vulkan_core.h>

VkResult CommandPool::initialize(
        std::shared_ptr<VulkanInfo> vkInfo,
        CommandPoolType type,
        VkCommandPoolCreateFlags flags,
        std::string name
) {
    m_vkInfo = vkInfo;

    uint32_t queueFamilyIndex = 0;
    switch (type) {
        case CommandPoolType::Graphics:
            queueFamilyIndex = vkInfo->graphicsQueueFamily;
            break;
        case CommandPoolType::Transfer:
            queueFamilyIndex = vkInfo->transferQueueFamily;
            break;
        default:
            spdlog::error("Invalid CommandPoolType");

            return VK_ERROR_INITIALIZATION_FAILED;
    }

    VkCommandPoolCreateInfo poolInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = flags,
        .queueFamilyIndex = queueFamilyIndex,
    };

    VkResult res;
    if ((res = vkCreateCommandPool(vkInfo->device, &poolInfo, nullptr, &m_pool)) != VK_SUCCESS) {
        return res;
    }
    Debug::SetObjectName(vkInfo->device, (U64)m_pool,
            VK_OBJECT_TYPE_COMMAND_POOL, name.c_str());

    m_name = name;

    return res;
}

void CommandPool::resizeBuffers(Size size) {
    // Free existing command buffers
    if (!m_buffers.empty()) {
        vkFreeCommandBuffers(m_vkInfo->device, m_pool, static_cast<uint32_t>(m_buffers.size()), m_buffers.data());
        m_buffers.clear();
    }

    if (size > 0) {
        m_buffers.resize(size);

        VkCommandBufferAllocateInfo allocInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = m_pool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = static_cast<uint32_t>(size),
        };

        VkResult result = vkAllocateCommandBuffers(m_vkInfo->device, &allocInfo, m_buffers.data());
        if (!VkUtils::checkVkResult(result, "Failed to allocate command buffers.")) {
            m_buffers.clear();
        }
    }

    for (Size i = 0; i < size; i++) {
        Debug::SetObjectName(m_vkInfo->device, (U64)m_buffers[i],
                VK_OBJECT_TYPE_COMMAND_BUFFER, fmt::format("{}'s Command Buffer {}", m_name, i).c_str());
    }
}

void CommandPool::resetPool() {
    VkResult result = vkResetCommandPool(m_vkInfo->device, getPool(), 0);
    if (!VkUtils::checkVkResult(result, "Error resetting command pool")) {
        return;
    }
}

VkCommandBuffer CommandPool::getBuffer(Size index) {
    if (index >= m_buffers.size()) {
        spdlog::error(
                "Attempt to getBuffer with index: {}, size of buffer array is {}",
                index, m_buffers.size());

        return VK_NULL_HANDLE;
    }

    return m_buffers[index];
}

std::vector<VkCommandBuffer>& CommandPool::getBuffers() {
    return m_buffers;
}

void CommandPool::shutdown() {
    if (!m_buffers.empty()) {
        vkFreeCommandBuffers(m_vkInfo->device, m_pool, static_cast<uint32_t>(m_buffers.size()), m_buffers.data());
        m_buffers.clear();
    }

    vkDestroyCommandPool(m_vkInfo->device, m_pool, nullptr);
    m_pool = VK_NULL_HANDLE;
}

