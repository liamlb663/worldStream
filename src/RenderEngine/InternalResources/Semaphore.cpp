// src/RenderEngine/InternalResources/Semaphore.hpp

#include "Semaphore.hpp"
#include "RenderEngine/Debug.hpp"
#include "RenderEngine/VkUtils.hpp"
#include "spdlog/spdlog.h"


bool Semaphore::initialize(std::shared_ptr<VulkanInfo> vkInfo, std::string name) {
    m_vkInfo = vkInfo;

    VkSemaphoreCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };

    VkResult result = vkCreateSemaphore(m_vkInfo->device, &createInfo, nullptr, &m_semaphore);
    if (!VkUtils::checkVkResult(result, "Failed to create Vulkan semaphore")) {
        return false;
    }

    if (!name.empty()) {
        Debug::SetObjectName(m_vkInfo->device, reinterpret_cast<uint64_t>(m_semaphore), VK_OBJECT_TYPE_SEMAPHORE, name.c_str());
    }

    return true;
}

void Semaphore::shutdown() {
    if (m_semaphore != VK_NULL_HANDLE) {
        vkDestroySemaphore(m_vkInfo->device, m_semaphore, nullptr);
        m_semaphore = VK_NULL_HANDLE;
        return;
    }
    spdlog::warn("Attempted to shutdown null semaphore");
}

VkSemaphore Semaphore::get() const {
    if (m_semaphore == VK_NULL_HANDLE)
        spdlog::warn("Attempted to get null semaphore");

    return m_semaphore;
}

