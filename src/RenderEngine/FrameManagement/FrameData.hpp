// src/RenderEngine/FrameData.hpp

#pragma once

#include "Core/DeletionQueue.hpp"
#include "Core/Types.hpp"
#include "../RenderResources/CommandPool.hpp"
#include "../VulkanInfo.hpp"

#include <memory>
#include <vulkan/vulkan.h>

class FrameData {
public:
    bool init(std::shared_ptr<VulkanInfo> vkInfo, Size frameNumber);
    void shutdown(std::shared_ptr<VulkanInfo> vkInfo);

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

private:

};

