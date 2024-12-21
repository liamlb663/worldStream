// src/RenderEngine/RenderEngine.hpp

#pragma once

#include "Core/DeletionQueue.hpp"
#include "RenderEngine/CommandSubmitter.hpp"
#include "RenderEngine/FrameData.hpp"
#include "VulkanInfo.hpp"
#include "Window.hpp"

#include <vector>
#include <vulkan/vulkan.h>

class RenderEngine {
public:
    bool initialize();
    void shutdown();

private:
    bool initVulkan();
    bool initFramedata();

    VulkanInfo m_vkInfo;
    Window* m_window;

    std::vector<FrameData> m_frameData;

    DeletionQueue m_mainDeletionQueue;

    CommandSubmitter m_commandSubmitter;
};

