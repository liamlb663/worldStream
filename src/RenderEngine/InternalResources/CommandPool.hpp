// src/RenderEngine/InternalResources/CommandPool.hpp

#pragma once

#include "../VulkanInfo.hpp"

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
            VulkanInfo* vkInfo,
            CommandPoolType type,
            VkCommandPoolCreateFlags flags,
            std::string name
    );

    void resizeBuffers(Size size);
    void resetPool();

    VkCommandPool getPool() { return m_pool; };
    VkCommandBuffer getBuffer(Size index);
    std::vector<VkCommandBuffer>& getBuffers();

    void shutdown();

private:
    VulkanInfo* m_vkInfo;
    VkCommandPool m_pool;
    std::vector<VkCommandBuffer> m_buffers;
    std::string m_name;
};

