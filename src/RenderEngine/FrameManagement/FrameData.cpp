// src/RenderEngine/FrameData.cpp

#include "FrameData.hpp"

#include "../VkUtils.hpp"
#include "../RenderGraph/GraphContext.hpp"
#include "spdlog/spdlog.h"

#include <fmt/core.h>

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

    m_currentWindowSize = size;

    return true;
}

void FrameData::shutdown() {
    deletionQueue.flush();

    for (Size i = 0; i < renderContext.semaphores.size(); i++) {
        renderContext.semaphores[i].shutdown();
    }
    for (Size i = 0; i < renderContext.images.size(); i++) {
        renderContext.images[i].shutdown();
    }
    renderGraph = nullptr;

    renderContext.shutdown();
    renderContext = {};

    commandPool.shutdown();

    renderFence.shutdown();
    swapchainSemaphore.shutdown();
    renderSemaphore.shutdown();
}

bool FrameData::regenerate(Vector<U32, 2> size) {
    renderContext.shutdown();

    renderContext = renderContext.create(m_vkInfo, renderGraph, size);
    m_currentWindowSize = size;

    return true;
}

void FrameData::changeRenderGraph(std::shared_ptr<RenderGraph> renderGraph) {
    renderContext.shutdown();
    this->renderGraph = renderGraph;
    renderContext.create(m_vkInfo, renderGraph, m_currentWindowSize);
}

