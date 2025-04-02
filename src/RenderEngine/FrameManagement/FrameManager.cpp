// src/RenderEngine/FrameManager.hpp

#include "FrameManager.hpp"

#include "../Config.hpp"

#include "RenderEngine/FrameSubmitInfo.hpp"
#include "RenderEngine/VkUtils.hpp"
#include "SwapchainManager.hpp"

#include <spdlog/spdlog.h>
#include <vulkan/vulkan.h>

bool FrameManager::initializeWindow(VulkanInfo* vkInfo) {
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
    vkDeviceWaitIdle(m_vkInfo->device);

    m_swapchain->shutdown();
    for (Size i = 0; i < Config::framesInFlight; i++) {
        m_frameData[i].shutdown();
    }
    m_window->shutdown(m_vkInfo->instance);
}

bool waitAndResetFences(VkDevice device, FrameData& frame, Size frameNumber) {
    VkFence renderFence = frame.renderFence.get();

    VkResult res = vkWaitForFences(device, 1, &renderFence, VK_TRUE, UINT64_MAX);
    if (!VkUtils::checkVkResult(res,
                fmt::format("Waiting on Frame {}'s render fence failed!", frameNumber))) {
        return false;
    }

    res = vkResetFences(device, 1, &renderFence);
    return VkUtils::checkVkResult(res,
            fmt::format("Resetting Frame {}'s render fence failed!", frameNumber));
}

U32 FrameManager::aquireNextSwap() {
    U32 index = 0;

    FrameData frame = m_frameData[m_frameNumber % Config::framesInFlight];

    bool waitSuccess = waitAndResetFences(
            m_vkInfo->device,
            frame,
            m_frameNumber % Config::framesInFlight
    );

    if (!waitSuccess) {
        spdlog::error("Error waiting on fences for frame[{}]",
                m_frameNumber % Config::framesInFlight);
        return -1;
    }

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

    return info;
}

void FrameManager::presentFrame(FrameSubmitInfo info) {
    VkSwapchainKHR swapchain = m_swapchain->getSwapchain();

    std::array<VkSemaphore, 1> semaphores = {
        info.frameData.renderContext.semaphores.back().get(),
    };

    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = static_cast<U32>(semaphores.size()),
        .pWaitSemaphores = semaphores.data(),
        .swapchainCount = 1,
        .pSwapchains = &swapchain,
        .pImageIndices = &info.swapchainImage.index,
        .pResults = nullptr,
    };

    VkResult presentResult = vkQueuePresentKHR(m_vkInfo->graphicsQueue, &presentInfo);
    if (presentResult == VK_ERROR_OUT_OF_DATE_KHR) {
        spdlog::error("VK_ERROR_OUT_OF_DATE_KHR hit during present");
        //TODO Resize
    }

    m_frameNumber++;
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

void FrameManager::addRenderObjects(Size geoId, std::vector<RenderObject> objects) {
    m_frameData[m_frameNumber % Config::framesInFlight]
        .addRenderObjects(geoId, objects);
}
