// src/RenderEngine/FrameManager.hpp

#pragma once

#include "../VulkanInfo.hpp"
#include "FrameData.hpp"
#include "../Commands/CommandSubmitter.hpp"
#include "RenderEngine/FrameManagement/SwapchainManager.hpp"
#include "Window.hpp"
#include <memory>

class FrameManager {
public:
    bool initializeWindow(std::shared_ptr<VulkanInfo> vkInfo);
    bool initializeFrames(std::shared_ptr<VulkanInfo> vkInfo);

    Window* getWindow() const { return m_window; }

    void transferSubmit(const std::function<void(VkCommandBuffer)>& function);

    void shutdown(std::shared_ptr<VulkanInfo> vkInfo);

private:
    Swapchain* m_swapchain;
    CommandSubmitter* m_commandSubmitter;
    Window* m_window;
    std::vector<FrameData> m_frameData;
    Size m_frameNumber;

    bool m_isResizing;
};

