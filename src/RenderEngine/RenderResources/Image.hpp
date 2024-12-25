// src/RenderEngine/RenderResources/Image.hpp

#pragma once

#include "../VulkanInfo.hpp"
#include "Core/Vector.hpp"

#include <vulkan/vulkan.h>

#include <memory>

class Image {
public:
    VkImage image = VK_NULL_HANDLE;
    VkImageView view = VK_NULL_HANDLE;
    VmaAllocation allocation = VK_NULL_HANDLE;
    Vector<U32, 3> size = {0,0,0};
    VkFormat format = VK_FORMAT_UNDEFINED;

    bool init(
            std::shared_ptr<VulkanInfo> vkInfo,
            const Vector<U32, 3>& imageSize,
            VkFormat imageFormat,
            VkImageUsageFlags usage,
            VkImageAspectFlags aspect
    );

    void shutdown();

private:
    std::shared_ptr<VulkanInfo> m_vkInfo;

};

