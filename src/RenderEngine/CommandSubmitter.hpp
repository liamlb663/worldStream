// src/RenderEngine/CommandSubmitter.hpp

#pragma once

#include "FrameData.hpp"
#include "VulkanInfo.hpp"

#include <vulkan/vulkan.h>

#include <functional>

class CommandSubmitter {
public:
    bool initialize(VulkanInfo vkInfo, std::vector<FrameData>* frames);

    void transferSubmit(const std::function<void(VkCommandBuffer)>& function);

    void shutdown();

private:
    VulkanInfo m_vkInfo;

    std::vector<FrameData>* m_frames;
    Size m_frameNumber;

};
