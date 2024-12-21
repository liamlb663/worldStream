// src/RenderEngine/FrameData.hpp

#pragma once

#include "Core/DeletionQueue.hpp"
#include "Core/Types.hpp"
#include "CommandPool.hpp"
#include "VulkanInfo.hpp"

#include <vulkan/vulkan.h>

#include <vector>

class FrameData {
public:
    bool init(VulkanInfo vkInfo, Size frameNumber);
    void shutdown();

    CommandPool commandPool;
    VkCommandBuffer transferBuffer;

    VkSemaphore swapchainSemaphore; // TODO: Replace with wrapper
    VkSemaphore renderSemaphore; // TODO: Replace with wrapper
    VkFence renderFence; // TODO: Replace with wrapper

    DeletionQueue deletionQueue;
    // TODO: Descriptor Allocator

    // TODO: Image drawImage;
    // TODO: Image depthImage;

    // TODO: DrawContext

    // TODO: Decide on new scene descriptor setup

    VkCommandBuffer getBuffer(Size index);

private:
    std::vector<VkCommandBuffer> m_commandBuffers;

};

