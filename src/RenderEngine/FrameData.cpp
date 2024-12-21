// src/RenderEngine/FrameData.cpp

#include "FrameData.hpp"
#include "Debug.hpp"
#include "RenderEngine/CommandPool.hpp"
#include "VkUtils.hpp"
#include "spdlog/fmt/bundled/format.h"

bool FrameData::init(VulkanInfo vkInfo, Size frameNumber) {
    // CommandPool
    VkUtils::checkVkResult(
            commandPool.initialize(vkInfo, CommandPoolType::Graphics),
            "Failed to initialize graphics commandPool");

    Debug::SetObjectName(
            vkInfo.device, (U64)commandPool.getPool(), VK_OBJECT_TYPE_COMMAND_POOL,
            fmt::format("Frame[{}]'s Command Pool", frameNumber).c_str());

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

void FrameData::shutdown() {


}

