// src/ResourceManagement/RenderResources/ImageView.hpp

#pragma once

#include "RenderEngine/VulkanInfo.hpp"
#include <vulkan/vulkan.h>

class ImageView {
public:
    static ImageView create(VulkanInfo* vkInfo, VkImageView imageView) {
        ImageView view;
        view.m_vkInfo = vkInfo;
        view.m_imageView = imageView;
        return view;
    }

    VkImageView get() const { return m_imageView; };
    void shutdown() {
        vkDestroyImageView(m_vkInfo->device, m_imageView, nullptr);
        m_vkInfo = nullptr;
        m_imageView = nullptr;
    };

private:
    VulkanInfo* m_vkInfo = nullptr;
    VkImageView m_imageView = nullptr;
};


