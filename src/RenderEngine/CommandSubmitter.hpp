// src/RenderEngine/CommandSubmitter.hpp

#pragma once

#include "VulkanInfo.hpp"
#include "FrameManagement/FrameData.hpp"

#include <vulkan/vulkan.h>

#include <functional>
#include <memory>

typedef struct SubmitInfo {
    Size frameNumber;
    FrameData frameData;
    //Swapchain image
} SubmitInfo;

class CommandSubmitter {
public:
    bool initialize(std::shared_ptr<VulkanInfo> vkInfo);

    void transferSubmit(const std::function<void(VkCommandBuffer)>& function);
    void frameSubmit(SubmitInfo info, const std::function<void(VkCommandBuffer)>& function);

    void shutdown();

private:
    std::shared_ptr<VulkanInfo> m_vkInfo;

};

