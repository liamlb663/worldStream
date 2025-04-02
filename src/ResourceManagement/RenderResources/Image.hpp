// src/ResourceManagement/RenderResources/Image.hpp

#pragma once

#include "RenderEngine/VulkanInfo.hpp"
#include "Core/Vector.hpp"

#include <vulkan/vulkan.h>

#include <string>

class Image {
public:
    VkImage image = VK_NULL_HANDLE;
    VkImageView view = VK_NULL_HANDLE;
    VmaAllocation allocation = VK_NULL_HANDLE;
    Vector<U32, 2> size = {0,0};
    VkFormat format = VK_FORMAT_UNDEFINED;
    VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;

    bool init(
            VulkanInfo* vkInfo,
            const Vector<U32, 2>& imageSize,
            VkFormat imageFormat,
            VkImageUsageFlags usage,
            std::string name
    );

    void shutdown();

private:
    VulkanInfo* m_vkInfo;

};

