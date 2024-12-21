// src/RenderEngine/CommandPool.hpp

#pragma once

#include "VulkanInfo.hpp"

#include <vulkan/vulkan.h>

#include <string>
#include <vector>

enum class CommandPoolType {
    Graphics,
    Transfer
};

class CommandPool {
public:
    CommandPool() : m_buffers() {};

    VkResult initialize(
            VulkanInfo vkInfo,
            CommandPoolType type,
            VkCommandPoolCreateFlags flags,
            std::string name
    );

    void resizeBuffers(Size size);

    VkCommandPool getPool() { return m_pool; };

    VkCommandBuffer getBuffer(Size index);

    void shutdown();

private:
    VkDevice m_device;
    VkCommandPool m_pool;
    std::vector<VkCommandBuffer> m_buffers;
    std::string m_name;
};

