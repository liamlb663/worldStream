// src/RenderEngine/FrameManager.hpp

#include "FrameManager.hpp"

#include "../Config.hpp"
#include "RenderEngine/FrameSubmitInfo.hpp"
#include "SwapchainManager.hpp"
#include "spdlog/spdlog.h"

bool FrameManager::initializeWindow(std::shared_ptr<VulkanInfo> vkInfo) {
    m_vkInfo = vkInfo;

    m_window = std::make_shared<Window>(1700, 900, "World Streamer");
    return m_window->init(vkInfo->instance);
}

bool FrameManager::initializeFrames() {
    m_isResizing = false;
    m_frameNumber = 0;

    m_frameData.resize(Config::framesInFlight);
    for (Size i = 0; i < Config::framesInFlight; i++) {
        if (!m_frameData[i].init(m_vkInfo, m_window->getSize(), i)) {
            spdlog::error("Failed to initialze Frame[{}]", i);
            return false;
        }
    }

    m_swapchain = std::make_shared<Swapchain>();
    m_swapchain->initialize(m_window, m_vkInfo);

    return true;
}

void FrameManager::shutdown() {
    m_swapchain->shutdown();
    for (Size i = 0; i < Config::framesInFlight; i++) {
        m_frameData[i].shutdown();
    }
    m_window->shutdown(m_vkInfo->instance);
}

U32 FrameManager::aquireNextSwap() {
    U32 index = 0;

    FrameData frame = m_frameData[m_frameNumber % Config::framesInFlight];
    Semaphore semaphore = frame.swapchainSemaphore;

    bool swapSuccess =
        m_swapchain->getNextImage(semaphore.get(), &index);

    if (!swapSuccess) {
        m_isResizing = true;

        waitOnFrames();

        m_swapchain->resizeSwapchain(m_window);

        // Regenerate frameData
        for (Size i = 0; i < m_frameData.size(); i++) {
            m_frameData[i].regenerate(m_swapchain->getSize());
        }

        m_isResizing = false;
        return aquireNextSwap();
    }

    return index;
}

SwapchainImage FrameManager::getSwapchainImage(U32 index) {
    return m_swapchain->getImage(index);
}

FrameSubmitInfo FrameManager::getNextFrameInfo() {
    U32 swapchainIndex = aquireNextSwap();
    SwapchainImage swapchainImage = getSwapchainImage(swapchainIndex);

    FrameSubmitInfo info = {
        .frameNumber = m_frameNumber,
        .frameData = m_frameData[m_frameNumber % Config::framesInFlight],
        .swapchainImage = swapchainImage,
    };

    m_frameNumber++;

    return info;
}

void FrameManager::setRenderGraph(std::shared_ptr<RenderGraph> renderGraph) {
    waitOnFrames();

    for (Size i = 0; i < m_frameData.size(); i++) {
        m_frameData[i].changeRenderGraph(renderGraph);
    }
}

void FrameManager::waitOnFrames() {
    for (Size i = 0; i < m_frameData.size(); i++) {
        VkFence fence = m_frameData[i].renderFence.get();
        vkWaitForFences(m_vkInfo->device, 1, &fence, VK_TRUE, UINT64_MAX);
    }
}

