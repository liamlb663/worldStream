// src/RenderEngine/FrameData.hpp

#pragma once

#include "Core/DeletionQueue.hpp"
#include "Core/Types.hpp"

#include "../VulkanInfo.hpp"

#include "../RenderResources/CommandPool.hpp"
#include "../RenderResources/Fence.hpp"
#include "../RenderResources/Semaphore.hpp"

#include <memory>
#include <vulkan/vulkan.h>

class FrameData {
public:
    bool init(std::shared_ptr<VulkanInfo> vkInfo, Size frameNumber);
    void shutdown();

    CommandPool commandPool;
    VkCommandBuffer transferBuffer;

    Semaphore swapchainSemaphore; // TODO: Replace with wrapper
    Semaphore renderSemaphore; // TODO: Replace with wrapper
    Fence renderFence; // TODO: Replace with wrapper

    DeletionQueue deletionQueue;
    // TODO: Descriptor Allocator

    // TODO: Image drawImage;
    // TODO: Image depthImage;

    // TODO: DrawContext

    // TODO: Decide on new scene descriptor setup

private:

};

