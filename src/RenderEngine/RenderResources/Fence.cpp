// src/RenderEngine/RenderResources/Fence.cpp

#include "Fence.hpp"

#include "../Debug.hpp"
#include "../VkUtils.hpp"
#include "spdlog/spdlog.h"

bool Fence::initialize(std::shared_ptr<VulkanInfo> vkInfo, bool signaled, std::string name) {
    m_vkInfo = vkInfo;

    VkFenceCreateFlags flags = 0;
    if (signaled) {
        flags = VK_FENCE_CREATE_SIGNALED_BIT;
    }

    VkFenceCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = flags
    };

    VkResult res = vkCreateFence(vkInfo->device, &info, nullptr, &m_fence);
    if (!VkUtils::checkVkResult(res, fmt::format("Error Creating Fence: {}", name))) {
        return false;
    }

    Debug::SetObjectName(vkInfo->device, (U64)m_fence, VK_OBJECT_TYPE_FENCE, name.c_str());

    return true;
}

void Fence::shutdown() {
    if (m_fence != VK_NULL_HANDLE) {
        vkDestroyFence(m_vkInfo->device, m_fence, nullptr);
        m_fence = VK_NULL_HANDLE;
        return;
    }
    spdlog::warn("Shutdown called on fence with NULL handle.");
}

bool Fence::wait(uint64_t timeout) {
    if (m_fence == VK_NULL_HANDLE) {
        spdlog::warn("Waiting on fence with NULL handle.");
        return false;
    }

    VkResult res = vkWaitForFences(m_vkInfo->device, 1, &m_fence, VK_TRUE, timeout);
    if (!VkUtils::checkVkResult(res, "Failed to wait on fence")) {
        return false;
    }

    return true;
}

bool Fence::reset() {
    if (m_fence == VK_NULL_HANDLE) {
        spdlog::warn("Attempted to reset an invalid fence handle");
        return false;
    }

    VkResult res = vkResetFences(m_vkInfo->device, 1, &m_fence);
    if (!VkUtils::checkVkResult(res, "Failed to reset fence")) {
        return false;
    }

    return true;
}

VkFence Fence::get() const {
    return m_fence;
}

