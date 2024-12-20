// src/RenderEngine/Debug.cpp

#include "Debug.hpp"

#include <VkBootstrap.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <iostream>
#include <unordered_set>

// Loaded Commands for Vulkan Debug Utils
static PFN_vkSetDebugUtilsObjectNameEXT my_vkSetDebugUtilsObjectNameEXT = nullptr;
static PFN_vkCmdBeginDebugUtilsLabelEXT my_vkCmdBeginDebugUtilsLabelEXT = nullptr;
static PFN_vkCmdEndDebugUtilsLabelEXT my_vkCmdEndDebugUtilsLabelEXT = nullptr;

// Load Util functions
void Debug::LoadDebugUtils(VkInstance instance) {

    //Object Labels
    my_vkSetDebugUtilsObjectNameEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(
        vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectNameEXT")
    );
    if (my_vkSetDebugUtilsObjectNameEXT == nullptr) {
        spdlog::error("Failed to load vkSetDebugUtilsObjectNameEXT");
    }

    //Start Command Buffer Labels
    my_vkCmdBeginDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdBeginDebugUtilsLabelEXT>(
        vkGetInstanceProcAddr(instance, "vkCmdBeginDebugUtilsLabelEXT")
    );
    if (my_vkCmdBeginDebugUtilsLabelEXT == nullptr) {
        std::cerr << "Failed to load vkCmdBeginDebugUtilsLabelEXT" << std::endl;
    }

    //End Command Buffer Labels
    my_vkCmdEndDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdEndDebugUtilsLabelEXT>(
        vkGetInstanceProcAddr(instance, "vkCmdEndDebugUtilsLabelEXT")
    );
    if (my_vkCmdEndDebugUtilsLabelEXT == nullptr) {
        std::cerr << "Failed to load vkCmdEndDebugUtilsLabelEXT" << std::endl;
    }
}

// Label Vulkan Object
void Debug::SetObjectName(VkDevice device, U64 objectHandle, VkObjectType objectType, const char* name) {

    if (my_vkSetDebugUtilsObjectNameEXT) {
        VkDebugUtilsObjectNameInfoEXT nameInfo = {};
        nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        nameInfo.pNext = nullptr;
        nameInfo.objectType = objectType;
        nameInfo.objectHandle = objectHandle;
        nameInfo.pObjectName = name;

        VkResult result = my_vkSetDebugUtilsObjectNameEXT(device, &nameInfo);
        if (result != VK_SUCCESS) {
            std::cerr << "Failed to set object name: " << name << std::endl;
        }
    } else {
        std::cerr << "my_vkSetDebugUtilsObjectNameEXT is not loaded" << std::endl;
    }
}

// Label command buffer
void Debug::SetCmdLabel(VkCommandBuffer cmd, glm::vec3 inColor, const char* name) {

    if (my_vkSetDebugUtilsObjectNameEXT) {
        VkDebugUtilsLabelEXT labelInfo = {};
        labelInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
        labelInfo.pLabelName = name;
        labelInfo.color[0] = inColor[0];
        labelInfo.color[1] = inColor[1];
        labelInfo.color[2] = inColor[2];
        labelInfo.color[3] = 1.0f;

        my_vkCmdBeginDebugUtilsLabelEXT(cmd, &labelInfo);
    } else {
       std::cerr << "my_vkCmdBeginDebugUtilsLabelEXT is not loaded" << std::endl;
    }
}

// Remove command buffer label
void Debug::RemoveCmdLabel(VkCommandBuffer cmd) {

    if (my_vkSetDebugUtilsObjectNameEXT) {
        my_vkCmdEndDebugUtilsLabelEXT(cmd);
    } else {
       std::cerr << "my_vkCmdEndDebugUtilsLabelEXT is not loaded" << std::endl;
    }
}

// Custom Debug Messenger
VkBool32 Debug::CustomDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    (void)pUserData;

    // Create a dedicated Vulkan logger with stderr sink
    static auto vulkan_logger = spdlog::stderr_color_mt("Vulkan");
    vulkan_logger->set_pattern("[%Y-%m-%d %T.%e] [%^%l%$: %n] [%v");
    vulkan_logger->set_level(spdlog::level::trace);

    auto type = vkb::to_string_message_type(messageType);

    constexpr const char* format = "{}]\n{}";

    static const std::unordered_set<U32> suppressed_message_ids = {
        0xfd92477a,  // vkAllocateMemory small allocation warning
        0x10b59d4b,  // vkBindBufferMemory small allocation warning
        0x675dc32e,  // VK_EXT_debug_utils usage warning
        0x44604b49,  // vkBindImageMemory small dedicated allocation warning
        0xc91ae640,  // vkEndCommandBuffer vertex buffer unused warning
        0xf4fc0180   // Transition Undefined to Shader read only
    };

    if (suppressed_message_ids.find(pCallbackData->messageIdNumber) != suppressed_message_ids.end()) {
        return VK_FALSE;  // Ignore this warning
    }

    // Set logging level based on message severity
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        vulkan_logger->error(format, type, pCallbackData->pMessage);
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        vulkan_logger->warn(format, type, pCallbackData->pMessage);
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        vulkan_logger->info(format, type, pCallbackData->pMessage);
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
        vulkan_logger->debug(format, type, pCallbackData->pMessage);
    }

    return VK_FALSE;  // You can return VK_TRUE if you want to stop the Vulkan call after logging
}

