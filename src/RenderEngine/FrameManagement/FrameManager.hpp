// src/RenderEngine/FrameManager.hpp

#pragma once

#include "../VulkanInfo.hpp"
#include "FrameData.hpp"
#include "../CommandSubmitter.hpp"
#include "RenderEngine/FrameManagement/SwapchainManager.hpp"
#include "Window.hpp"
#include <memory>

class FrameManager {
public:
    bool initializeWindow(std::shared_ptr<VulkanInfo> vkInfo);
    bool initializeFrames(std::shared_ptr<VulkanInfo> vkInfo);
    void shutdown(std::shared_ptr<VulkanInfo> vkInfo);

    std::shared_ptr<Window> getWindow() const { return m_window; }

    void transferSubmit(const std::function<void(VkCommandBuffer)>& function);

    U32 aquireNextSwap() {
        U32 index = 0;

        bool swapSuccess =
            m_swapchain->getNextImage(m_frameData[m_frameNumber].swapchainSemaphore, &index);

        if (!swapSuccess) {
            m_isResizing = true;
            m_swapchain->resizeSwapchain(m_window);

            //TODO regenerate frameData

            return aquireNextSwap();
        }

        return index;
    }

    SwapchainImage getSwapchainImage(U32 index) {
        return m_swapchain->getImage(index);
    }


private:
    std::shared_ptr<Swapchain> m_swapchain;
    std::shared_ptr<CommandSubmitter> m_commandSubmitter;
    std::shared_ptr<Window> m_window;
    std::vector<FrameData> m_frameData;
    Size m_frameNumber;

    bool m_isResizing;
};

