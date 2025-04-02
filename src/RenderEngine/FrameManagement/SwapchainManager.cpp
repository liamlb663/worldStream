// src/RenderEngine/SwapchainManager.cpp

#include "SwapchainManager.hpp"
#include "../Config.hpp"
#include "../VulkanInfo.hpp"
#include "Core/Vector.hpp"
#include "Window.hpp"
#include <cstdint>


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
    vkb::SwapchainBuilder swapchainBuilder = vkb::SwapchainBuilder(m_vkInfo->physicalDevice, m_vkInfo->device, window->getSurface());

    VkSurfaceFormatKHR surfaceFormat;
    surfaceFormat.format = Config::swapchainFormat;
    surfaceFormat.colorSpace = Config::swapchainColorSpace;

    auto swapchainReturn = swapchainBuilder
        .set_desired_format(surfaceFormat)
        .set_desired_present_mode(Config::swapchainPresentMode)
        .set_desired_extent(
                window->getSize().value.x,
                window->getSize().value.y)
        .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
        .build();

    if (!swapchainReturn) {
        spdlog::error("Failed to create swapchain, Error: {}", swapchainReturn.error().message());
        return false;
    }

    vkb::Swapchain vkbSwapchain = swapchainReturn.value();

    m_size = Vector<U32, 2>::fromExtent2D(vkbSwapchain.extent);
    m_swapchain = vkbSwapchain.swapchain;
    m_images = vkbSwapchain.get_images().value();
    m_imageViews = vkbSwapchain.get_image_views().value();

    return true;
}

void Swapchain::destroySwapchain() {
    vkDestroySwapchainKHR(m_vkInfo->device, m_swapchain, nullptr);

    for (size_t i = 0; i < m_imageViews.size(); i++) {
        vkDestroyImageView(m_vkInfo->device, m_imageViews[i], nullptr);
    }
}
