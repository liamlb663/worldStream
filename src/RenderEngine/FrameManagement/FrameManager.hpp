// src/RenderEngine/FrameManager.hpp

#pragma once

#include "FrameData.hpp"
#include "RenderEngine/RenderObjects/TextureRenderObject.hpp"
#include "SwapchainManager.hpp"
#include "Window.hpp"

#include "../VulkanInfo.hpp"
#include "../FrameSubmitInfo.hpp"
#include "../RenderGraph/RenderGraph.hpp"
#include "../RenderObjects/RenderObject.hpp"

#include <vulkan/vulkan.h>

class FrameManager {
public:
    bool initializeWindow(VulkanInfo* vkInfo);
    bool initializeFrames();
    void shutdown();

    std::shared_ptr<Window> getWindow() const { return m_window; }

    U32 aquireNextSwap();
    SwapchainImage getSwapchainImage(U32 index);
    FrameSubmitInfo getNextFrameInfo();
    void presentFrame(FrameSubmitInfo info);

    void addRenderObjects(Size geoId, std::vector<RenderObject> objects);
    void addTextureRenderObjects(std::vector<TextureRenderObject> objects);

    GLFWwindow* getGLFWwindow() const { return m_window->getGLFWwindow(); };
    void setRenderGraph(std::shared_ptr<RenderGraph> renderGraph);

    void waitOnFrames();

private:
    VulkanInfo* m_vkInfo;
    std::shared_ptr<Swapchain> m_swapchain;
    std::shared_ptr<Window> m_window;
    std::vector<FrameData> m_frameData;
    Size m_frameNumber;

    bool m_isResizing;

};

