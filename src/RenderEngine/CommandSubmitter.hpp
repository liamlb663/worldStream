// src/RenderEngine/CommandSubmitter.hpp

#pragma once

#include "FrameSubmitInfo.hpp"
#include "VulkanInfo.hpp"

#include <vulkan/vulkan.h>

#include <functional>

class CommandSubmitter {
public:
    bool initialize(VulkanInfo* vkInfo);

    void transferSubmit(const std::function<void(VkCommandBuffer)>& function);
    void frameSubmit(FrameSubmitInfo info);

    void transitionVulkanImage(VkCommandBuffer cmd, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, bool isTransferQueue = false);
    void transitionImage(VkCommandBuffer cmd, Image* image, VkImageLayout newLayout, bool isTransferQueue = false);

    void shutdown();

private:
    VulkanInfo* m_vkInfo;

};

