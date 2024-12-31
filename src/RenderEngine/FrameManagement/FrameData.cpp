// src/RenderEngine/FrameData.cpp

#include "FrameData.hpp"
#include "../VkUtils.hpp"

#include "RenderEngine/Config.hpp"
#include "fmt/core.h"
#include "spdlog/spdlog.h"

bool FrameData::createImages(Vector<U32, 2> size) {
    VkImageUsageFlags commonFlags = 0;
    commonFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    commonFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    commonFlags |= VK_IMAGE_USAGE_STORAGE_BIT;
    commonFlags |= VK_IMAGE_USAGE_SAMPLED_BIT;

    bool result = drawImage.init(
            m_vkInfo,
            Vector<U32, 2>(size.value.x, size.value.y),
            Config::drawFormat,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | commonFlags,
            fmt::format("Frame[{}]'s Draw Image", m_frameNumber)
    );
    if (!result) {
        spdlog::error("Draw Image failed to initialize");
        return false;
    }

    result = depthImage.init(
            m_vkInfo,
            Vector<U32, 2>(size.value.x, size.value.y),
            Config::depthFormat,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | commonFlags,
            fmt::format("Frame[{}]'s Depth Image", m_frameNumber)
    );
    if (!result) {
        spdlog::error("Depth Image failed to initialize");
        return false;
    }

    return true;
}

bool FrameData::init(std::shared_ptr<VulkanInfo> vkInfo, Vector<U32, 2> size, Size frameNumber) {
    m_vkInfo = vkInfo;
    m_frameNumber = frameNumber;

    // CommandPool
    VkUtils::checkVkResult(
            commandPool.initialize(vkInfo, CommandPoolType::Graphics, 0,
                fmt::format("Frame[{}]'s Graphics Command Pool", frameNumber)),
            "Failed to initialize graphics commandPool");

    // Transfer Buffer
    transferBuffer = vkInfo->transferPool->getBuffer(frameNumber);

    // Semaphores
    if (!renderSemaphore.initialize(vkInfo,
            fmt::format("Failed to Create Frame[{}]'s Swapchain Semaphore", frameNumber).c_str())) {
        return false;
    }
    if (!swapchainSemaphore.initialize(vkInfo,
            fmt::format("Frame[{}]'s Swapchain Semaphore", frameNumber).c_str())) {
        return false;
    }

    // Fence
    if (!renderFence.initialize(vkInfo, true,
            fmt::format("Frame[{}]'s Render Fence", frameNumber))) {
        return false;
    }

    // Images
    if (!createImages(size)) {
        return false;
    }

    return true;
}

void FrameData::shutdown() {
    deletionQueue.flush();

    depthImage.shutdown();
    drawImage.shutdown();

    commandPool.shutdown();

    renderFence.shutdown();
    swapchainSemaphore.shutdown();
    renderSemaphore.shutdown();
}

bool FrameData::regenerate(Vector<U32, 2> size) {
    depthImage.shutdown();
    drawImage.shutdown();

    if (!createImages(size)) {
        return false;
    }

    return true;
}

