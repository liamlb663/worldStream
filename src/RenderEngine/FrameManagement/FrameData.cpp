// src/RenderEngine/FrameData.cpp

#include "FrameData.hpp"
#include "../VkUtils.hpp"

#include "fmt/core.h"

bool FrameData::init(std::shared_ptr<VulkanInfo> vkInfo, Size frameNumber) {
    // CommandPool
    VkUtils::checkVkResult(
            commandPool.initialize(vkInfo, CommandPoolType::Graphics, 0,
                fmt::format("Frame[{}]'s Graphics Command Pool", frameNumber)),
            "Failed to initialize graphics commandPool");

    // Transfer Buffer
    transferBuffer = vkInfo->transferPool->getBuffer(frameNumber);

    // Semaphores
    renderSemaphore.initialize(vkInfo, fmt::format("Failed to Create Frame[{}]'s Swapchain Semaphore", frameNumber).c_str());
    swapchainSemaphore.initialize(vkInfo, fmt::format("Frame[{}]'s Swapchain Semaphore", frameNumber).c_str());

    //Fence
    renderFence.initialize(vkInfo, true, fmt::format("Frame[{}]'s Render Fence", frameNumber));

    return true;
}

void FrameData::shutdown() {
    deletionQueue.flush();

    commandPool.shutdown();

    renderFence.shutdown();
    swapchainSemaphore.shutdown();
    renderSemaphore.shutdown();
}

