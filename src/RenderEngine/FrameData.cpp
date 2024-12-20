// src/RenderEngine/FrameData.cpp

#include "FrameData.hpp"
#include "Debug.hpp"
#include "VkUtils.hpp"

bool FrameData::init(VulkanInfo vkInfo) {
    // CommandPool
    VkCommandPoolCreateInfo cmdPoolInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = vkInfo.graphicsQueueFamily,
    };

    VkResult res = vkCreateCommandPool(
            vkInfo.device, &cmdPoolInfo, nullptr, &commandPool);
    if (!VkUtils::checkVkResult(res, "Failed to Create Frame's Command Pool"))
        return false;

    Debug::SetObjectName(
            vkInfo.device, (U64)commandPool, VK_OBJECT_TYPE_COMMAND_POOL,
            "Frame Command Pool");

    // Semaphores
    VkSemaphoreCreateInfo semaCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };

    res = vkCreateSemaphore(
            vkInfo.device, &semaCreateInfo, nullptr, &renderSemaphore);
    if (!VkUtils::checkVkResult(res, "Failed to Create Frame's Render Semaphore"))
        return false;

    Debug::SetObjectName(
            vkInfo.device, (U64)renderSemaphore, VK_OBJECT_TYPE_SEMAPHORE,
            "Frame Render Semaphore");

    res = vkCreateSemaphore(
            vkInfo.device, &semaCreateInfo, nullptr, &swapchainSemaphore);
    if (!VkUtils::checkVkResult(res, "Failed to Create Frame's Swapchain Semaphore"))
        return false;

    Debug::SetObjectName(
            vkInfo.device, (U64)renderSemaphore, VK_OBJECT_TYPE_SEMAPHORE,
            "Frame Swapchain Semaphore");

    //Fence
    VkFenceCreateInfo fenceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    res = vkCreateFence(
            vkInfo.device, &fenceCreateInfo, nullptr, &renderFence);
    if (!VkUtils::checkVkResult(res, "Failed to Create Frame's Render Fence"))
        return false;

    Debug::SetObjectName(
            vkInfo.device, (U64)renderFence, VK_OBJECT_TYPE_FENCE,
            "Frame Render Fence");

    return true;
}

void FrameData::shutdown() {


}

