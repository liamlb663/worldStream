// src/RenderEngine/FrameManager.hpp

#include "FrameManager.hpp"

#include "../Config.hpp"
#include "SwapchainManager.hpp"
#include "spdlog/spdlog.h"

bool FrameManager::initializeWindow(std::shared_ptr<VulkanInfo> vkInfo) {
    m_window = new Window(1700, 900, "World Streamer");
    return m_window->init(vkInfo->instance);
}

bool FrameManager::initializeFrames(std::shared_ptr<VulkanInfo> vkInfo) {
    m_isResizing = false;
    m_frameNumber = 0;

    m_frameData.resize(Config::framesInFlight);
    for (Size i = 0; i < Config::framesInFlight; i++) {
        if (!m_frameData[i].init(vkInfo, i)) {
            spdlog::error("Failed to initialze Frame[{}]", i);
            return false;
        }
    }

    m_commandSubmitter = new CommandSubmitter();
    if (!m_commandSubmitter->initialize()) {
        spdlog::error("Failed to initialze CommandSubmitter");
        return false;
    }

    m_swapchain = new Swapchain();

    return true;
}

void FrameManager::transferSubmit(const std::function<void(VkCommandBuffer)>& function) {
    m_commandSubmitter->transferSubmit(m_frameNumber, function);
}

void FrameManager::shutdown(std::shared_ptr<VulkanInfo> vkInfo) {
    m_commandSubmitter->shutdown();
    for (Size i = 0; i < Config::framesInFlight; i++) {
        m_frameData[i].shutdown(vkInfo);
    }
}
