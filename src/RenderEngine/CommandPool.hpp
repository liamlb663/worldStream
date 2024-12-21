// src/RenderEngine/CommandPool.hpp

#pragma once

#include "RenderEngine/VkUtils.hpp"
#include "VulkanInfo.hpp"
#include "spdlog/spdlog.h"

#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

enum class CommandPoolType {
    Graphics,
    Transfer
};

class CommandPool {
public:
    CommandPool() : m_buffers() {};

    VkResult initialize(VulkanInfo vkInfo, CommandPoolType type, VkCommandPoolCreateFlags flags = 0) {
        m_device = vkInfo.device;

        uint32_t queueFamilyIndex = 0;
        switch (type) {
            case CommandPoolType::Graphics:
                queueFamilyIndex = vkInfo.graphicsQueueFamily;
                break;
            case CommandPoolType::Transfer:
                queueFamilyIndex = vkInfo.transferQueueFamily;
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

        return vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_pool);
    }

    void resizeBuffers(Size size) {
        // Free existing command buffers
        if (!m_buffers.empty()) {
            vkFreeCommandBuffers(m_device, m_pool, static_cast<uint32_t>(m_buffers.size()), m_buffers.data());
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

            VkResult result = vkAllocateCommandBuffers(m_device, &allocInfo, m_buffers.data());
            if (!VkUtils::checkVkResult(result, "Failed to allocate command buffers.")) {
                m_buffers.clear();
            }
        }
    }

    VkCommandPool getPool() {
        return m_pool;
    }

    VkCommandBuffer getBuffer(Size index) {
        if (m_buffers.size() > index) return VK_NULL_HANDLE;

        return m_buffers[index];
    }

    void shutdown() {
        if (!m_buffers.empty()) {
            vkFreeCommandBuffers(m_device, m_pool, static_cast<uint32_t>(m_buffers.size()), m_buffers.data());
            m_buffers.clear();
        }

        vkDestroyCommandPool(m_device, m_pool, nullptr);
    }

private:
    VkDevice m_device;
    VkCommandPool m_pool;
    std::vector<VkCommandBuffer> m_buffers;

};

