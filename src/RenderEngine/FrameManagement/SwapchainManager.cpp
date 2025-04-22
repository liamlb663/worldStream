// src/RenderEngine/SwapchainManager.cpp

#include "SwapchainManager.hpp"
#include "../Config.hpp"
#include "../VulkanInfo.hpp"
#include "Core/Vector.hpp"
#include "Window.hpp"
#include <cstdint>
#include <algorithm>

bool Swapchain::initialize(
        std::shared_ptr<Window> window,
        VulkanInfo* vkInfo
) {
    m_vkInfo = vkInfo;

    return createSwapchain(window);
}

void Swapchain::shutdown() {
    destroySwapchain();
}

bool Swapchain::resizeSwapchain(std::shared_ptr<Window> window) {
    vkDeviceWaitIdle(m_vkInfo->device);

    destroySwapchain();
    return createSwapchain(window);
}

bool Swapchain::getNextImage(VkSemaphore semaphore, U32* index) {
    U64 timeout = UINT64_MAX;

    VkResult result = vkAcquireNextImageKHR(
            m_vkInfo->device,
            m_swapchain,
            timeout,
            semaphore,
            nullptr,
            index
    );

    if (result == VK_ERROR_OUT_OF_DATE_KHR ||
        result == VK_SUBOPTIMAL_KHR) {
        return false;
    }

    return true;
}

bool Swapchain::createSwapchain(std::shared_ptr<Window> window) {
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_vkInfo->physicalDevice, window->getSurface(), &capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_vkInfo->physicalDevice, window->getSurface(), &formatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_vkInfo->physicalDevice, window->getSurface(), &formatCount, formats.data());

    VkSurfaceFormatKHR surfaceFormat = formats[0];
    for (const auto& availableFormat : formats) {
        if (availableFormat.format == Config::swapchainFormat &&
            availableFormat.colorSpace == Config::swapchainColorSpace) {
            surfaceFormat = availableFormat;
            break;
        }
    }

    VkExtent2D extent = {
        std::clamp(window->getSize().value.x, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
        std::clamp(window->getSize().value.y, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
    };

    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
        imageCount = capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = window->getSurface();

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    uint32_t queueFamilyIndices[] = {
        m_vkInfo->graphicsQueueFamily,
        m_vkInfo->transferQueueFamily
    };

    if (m_vkInfo->graphicsQueueFamily != m_vkInfo->transferQueueFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = Config::swapchainPresentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(m_vkInfo->device, &createInfo, nullptr, &m_swapchain) != VK_SUCCESS) {
        spdlog::error("Failed to create swapchain");
        return false;
    }

    m_size = Vector<U32, 2>::fromExtent2D(extent);

    uint32_t imageCountOut;
    vkGetSwapchainImagesKHR(m_vkInfo->device, m_swapchain, &imageCountOut, nullptr);
    m_images.resize(imageCountOut);
    vkGetSwapchainImagesKHR(m_vkInfo->device, m_swapchain, &imageCountOut, m_images.data());

    m_imageViews.resize(m_images.size());
    for (size_t i = 0; i < m_images.size(); i++) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = m_images[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = surfaceFormat.format;
        viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(m_vkInfo->device, &viewInfo, nullptr, &m_imageViews[i]) != VK_SUCCESS) {
            spdlog::error("Failed to create image view for swapchain image");
            return false;
        }
    }

    return true;
}

void Swapchain::destroySwapchain() {
    for (size_t i = 0; i < m_imageViews.size(); i++) {
        vkDestroyImageView(m_vkInfo->device, m_imageViews[i], nullptr);
    }
    vkDestroySwapchainKHR(m_vkInfo->device, m_swapchain, nullptr);
}

