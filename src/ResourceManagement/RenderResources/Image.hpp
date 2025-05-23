// src/ResourceManagement/RenderResources/Image.hpp

#pragma once

#include "RenderEngine/VulkanInfo.hpp"
#include "Core/Vector.hpp"
#include "ResourceManagement/RenderResources/ImageView.hpp"

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
    U32 layers = 0;

    bool init(
            VulkanInfo* vkInfo,
            const Vector<U32, 2>& imageSize,
            VkFormat imageFormat,
            VkImageUsageFlags usage,
            U32 layers,
            VkImageCreateFlags flags,
            VkImageViewType viewType,
            std::string name
    );

    ImageView createLayerView(
        U32 layerIndex,
        const std::string& name
    ) const;

    ImageView getImageView() const;

    void shutdown();

private:
    VulkanInfo* m_vkInfo;

};

