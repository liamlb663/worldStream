// src/RenderEngine/CommandSubmitter.hpp

#pragma once

#include "FrameSubmitInfo.hpp"
#include "VulkanInfo.hpp"

#include <vulkan/vulkan.h>

#include <functional>
#include <memory>

class CommandSubmitter {
public:
    bool initialize(std::shared_ptr<VulkanInfo> vkInfo);

    void transferSubmit(const std::function<void(VkCommandBuffer)>& function);
    void frameSubmit(FrameSubmitInfo info);

    void transitionImage(VkCommandBuffer cmd, std::shared_ptr<Image> image, VkImageLayout newLayout);

    void shutdown();

private:
    std::shared_ptr<VulkanInfo> m_vkInfo;

};

