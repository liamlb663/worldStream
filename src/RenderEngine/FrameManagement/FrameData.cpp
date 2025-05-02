// src/RenderEngine/FrameData.cpp

#include "FrameData.hpp"

#include "../VkUtils.hpp"
#include "RenderEngine/RenderObjects/TextureRenderObject.hpp"

#include <fmt/core.h>

bool FrameData::init(VulkanInfo* vkInfo, Vector<U32, 2> size, Size frameNumber) {
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

    renderGraph = nullptr;
    renderContext.shutdown();

    commandPool.shutdown();

    renderFence.shutdown();
    swapchainSemaphore.shutdown();
    renderSemaphore.shutdown();
}

bool FrameData::regenerate(Vector<U32, 2> size) {
    m_currentWindowSize = size;
    renderContext.shutdown();
    renderContext = RenderInfo::create(m_vkInfo, renderGraph, size);

    return true;
}

void FrameData::changeRenderGraph(std::shared_ptr<RenderGraph> renderGraph) {
    if (renderGraph != nullptr) renderContext.shutdown();
    this->renderGraph = renderGraph;
    renderContext = RenderInfo::create(
            m_vkInfo,
            renderGraph,
            m_currentWindowSize
    );
    commandPool.resizeBuffers(renderGraph->nodes.size());
}

void FrameData::addRenderObjects(Size geoId, std::vector<RenderObject> objects) {
    renderContext.geometries[geoId].insert(
            renderContext.geometries[geoId].end(), objects.begin(), objects.end());
}

void FrameData::clearRenderObjects(Size geoId) {
    renderContext.geometries[geoId].clear();
}

void FrameData::clearAllRenderObjects() {
    for (Size i = 0; i < renderContext.geometries.size(); i++) {
        clearRenderObjects(i);
    }
}

void FrameData::addTextureTargets(std::vector<TextureRenderObject> targets) {
    renderContext.textureTargets.insert(
            renderContext.textureTargets.end(), targets.begin(), targets.end());
}

void FrameData::clearTextureTargets() {
    renderContext.textureTargets.clear();
}

