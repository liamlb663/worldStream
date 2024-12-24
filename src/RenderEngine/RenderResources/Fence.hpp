// src/RenderEngine/RenderResources/Fence.hpp

#pragma once

#include "RenderEngine/Debug.hpp"
#include "RenderEngine/VkUtils.hpp"

#include <cstdint>
#include <vulkan/vulkan.h>
#include <fmt/core.h>
#include <vulkan/vulkan_core.h>

class Fence {
public:
    Fence(VkFence fence): m_fence(fence) {}

    Fence createFence(VkDevice device, bool signaled, std::string name) {
        VkFenceCreateFlags flags = 0;
        if (signaled) {
            flags = VK_FENCE_CREATE_SIGNALED_BIT;
        }

        VkFenceCreateInfo info = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = flags
        };

        VkFence fence;
        VkResult res = vkCreateFence(device, &info, nullptr, &fence);
        if (!VkUtils::checkVkResult(res, fmt::format("Error Creating Fence: {}", name))) {
            return nullptr;
        }

        Debug::SetObjectName(device, (U64)fence, VK_OBJECT_TYPE_FENCE, name.c_str());

        return Fence(fence);
    }

    void shutdown(VkDevice device) {
        if (m_fence != VK_NULL_HANDLE) {
            vkDestroyFence(device, m_fence, nullptr);
        }
    }

    bool wait(VkDevice device, uint64_t timeout = UINT64_MAX) {
        VkResult res = vkResetFences(device, 1, &m_fence);
    }

private:
    VkFence m_fence = VK_NULL_HANDLE;
};

