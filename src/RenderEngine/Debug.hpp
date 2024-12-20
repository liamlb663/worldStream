// src/RenderEngine/debug.hpp

#pragma once

#include "Core/Types.hpp"

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

namespace Debug {
    void SetObjectName(VkDevice device, U64 objectHandle, VkObjectType objectType, const char* name);
    void LoadDebugUtils(VkInstance instance);
    void SetCmdLabel(VkCommandBuffer cmd, glm::vec3 color, const char* name);
    void RemoveCmdLabel(VkCommandBuffer cmd);

    VkBool32 CustomDebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
    );
}
