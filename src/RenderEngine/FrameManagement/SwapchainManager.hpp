// src/RenderEngine/SwapchainManager.hpp

#pragma once

#include "Core/Vector.hpp"
#include "Window.hpp"
#include "../VulkanInfo.hpp"

#include <VkBootstrap.h>
#include <spdlog/spdlog.h>
#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

typedef struct SwapchainImage {
    VkImage image;
    VkImageView imageView;
    Vector<U32, 2> size;
    U32 index;
} SwapchainImage;

class Swapchain {
public:
    bool initialize(
        std::shared_ptr<Window> window,
        VulkanInfo* vkInfo
    );
    void shutdown();

    bool resizeSwapchain(std::shared_ptr<Window> window);
    bool getNextImage(VkSemaphore semaphore, U32* index);

    VkSwapchainKHR getSwapchain() { return m_swapchain; }

    SwapchainImage getImage(Size index) {
        return {
            .image = m_images[index],
            .imageView = m_imageViews[index],
            .size = m_size,
            .index = static_cast<U32>(index),
        };
    }

    Vector<U32, 2> getSize() { return m_size; }

private:
    VulkanInfo* m_vkInfo;

    VkSwapchainKHR m_swapchain;

    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;
    Vector<U32, 2> m_size;

    bool createSwapchain(std::shared_ptr<Window> window);
    void destroySwapchain();
};

