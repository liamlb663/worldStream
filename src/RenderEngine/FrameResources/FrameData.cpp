// src/RenderEngine/FrameData.cpp

#include "FrameData.hpp"
#include "../Debug.hpp"
#include "../VkUtils.hpp"
#include "../Commands/CommandPool.hpp"

#include "fmt/core.h"

bool FrameData::init(VulkanInfo vkInfo, Size frameNumber) {
    // CommandPool
    VkUtils::checkVkResult(
            commandPool.initialize(vkInfo, CommandPoolType::Graphics, 0,
                fmt::format("Frame[{}]'s Graphics Command Pool", frameNumber)),
            "Failed to initialize graphics commandPool");

    // Transfer Buffer
    transferBuffer = vkInfo.transferPool->getBuffer(frameNumber);

    // Semaphores
    VkSemaphoreCreateInfo semaCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };

    // Render Semaphore
    VkResult res = vkCreateSemaphore(
            vkInfo.device, &semaCreateInfo, nullptr, &renderSemaphore);
    if (!VkUtils::checkVkResult(res,
            fmt::format("Failed to Create Frame[{}]'s Render Semaphore", frameNumber).c_str()))
        return false;

    Debug::SetObjectName(
            vkInfo.device, (U64)renderSemaphore, VK_OBJECT_TYPE_SEMAPHORE,
            fmt::format("Frame[{}]'s Render Semaphore", frameNumber).c_str());

    // Swapchain Semaphore
    res = vkCreateSemaphore(
            vkInfo.device, &semaCreateInfo, nullptr, &swapchainSemaphore);
    if (!VkUtils::checkVkResult(res,
            fmt::format("Failed to Create Frame[{}]'s Swapchain Semaphore", frameNumber).c_str()))
        return false;

    Debug::SetObjectName(
            vkInfo.device, (U64)swapchainSemaphore, VK_OBJECT_TYPE_SEMAPHORE,
            fmt::format("Frame[{}]'s Swapchain Semaphore", frameNumber).c_str());

    //Fence
    VkFenceCreateInfo fenceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    res = vkCreateFence(
            vkInfo.device, &fenceCreateInfo, nullptr, &renderFence);
    if (!VkUtils::checkVkResult(res,
            fmt::format("Failed to Create Frame[{}]'s Render Fence", frameNumber).c_str()))
        return false;

    Debug::SetObjectName(
            vkInfo.device, (U64)renderFence, VK_OBJECT_TYPE_FENCE,
            fmt::format("Frame[{}]'s Render Fence", frameNumber).c_str());

    return true;
}

void FrameData::shutdown(VulkanInfo vkInfo) {
    deletionQueue.flush();

    commandPool.shutdown();

    vkDestroyFence(vkInfo.device, renderFence, nullptr);
    vkDestroySemaphore(vkInfo.device, renderSemaphore, nullptr);
    vkDestroySemaphore(vkInfo.device, swapchainSemaphore, nullptr);
}

