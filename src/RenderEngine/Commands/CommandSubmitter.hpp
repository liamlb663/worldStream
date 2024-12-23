// src/RenderEngine/CommandSubmitter.hpp

#pragma once

#include "../VulkanInfo.hpp"
#include "RenderEngine/FrameManagement/FrameData.hpp"

#include <vulkan/vulkan.h>

#include <functional>

typedef struct SubmitInfo {
    Size frameNumber;
    FrameData frameData;
    //Swapchain image
} SubmitInfo;

class CommandSubmitter {
public:
    bool initialize();

    void transferSubmit(Size frameNumber, const std::function<void(VkCommandBuffer)>& function);
    void frameSubmit(SubmitInfo info, const std::function<void(VkCommandBuffer)>& function);

    void shutdown();

private:
    VulkanInfo m_vkInfo;

};

