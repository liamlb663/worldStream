// src/RenderEngine/FrameManager.hpp

#pragma once

#include "../VulkanInfo.hpp"
#include "FrameData.hpp"
#include "RenderEngine/FrameSubmitInfo.hpp"
#include "RenderEngine/RenderGraph/RenderGraph.hpp"
#include "SwapchainManager.hpp"
#include "Window.hpp"

#include <vulkan/vulkan.h>

#include <memory>

class FrameManager {
public:
    bool initializeWindow(std::shared_ptr<VulkanInfo> vkInfo);
    bool initializeFrames();
    void shutdown();

    std::shared_ptr<Window> getWindow() const { return m_window; }

    U32 aquireNextSwap();
    SwapchainImage getSwapchainImage(U32 index);
    FrameSubmitInfo getNextFrameInfo();

    void setRenderGraph(std::shared_ptr<RenderGraph> renderGraph);

    void waitOnFrames();

private:
    std::shared_ptr<VulkanInfo> m_vkInfo;
    std::shared_ptr<Swapchain> m_swapchain;
    std::shared_ptr<Window> m_window;
    std::vector<FrameData> m_frameData;
    Size m_frameNumber;

    bool m_isResizing;

};

