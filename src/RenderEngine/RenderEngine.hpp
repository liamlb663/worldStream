// src/RenderEngine/RenderEngine.hpp

#pragma once

#include "Core/DeletionQueue.hpp"
#include "FrameManagement/FrameManager.hpp"
#include "CommandSubmitter.hpp"
#include "VulkanInfo.hpp"

#include <memory>
#include <vulkan/vulkan.h>

class RenderEngine {
public:
    bool initialize();
    void shutdown();

    std::shared_ptr<VulkanInfo> getInfo() const;
    std::shared_ptr<CommandSubmitter> getSubmitter() const;

private:
    bool initVulkan();
    bool initFramedata();

    std::shared_ptr<VulkanInfo> m_vkInfo;

    std::shared_ptr<FrameManager> m_frameManager;
    std::shared_ptr<CommandSubmitter> m_commandSubmitter;

    DeletionQueue m_mainDeletionQueue;

};

