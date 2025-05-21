// src/ResourceManagement/RenderResources/SparseBuffer.hpp

#pragma once

#include "Core/Vector.hpp"
#include "RenderEngine/VulkanInfo.hpp"
#include <unordered_map>
#include <vulkan/vulkan.h>
#include <utility>
#include <functional>

namespace std {
    template <>
    struct hash<std::pair<U32, U32>> {
        size_t operator()(const std::pair<U32, U32>& p) const {
            return std::hash<U32>()(p.first) ^ (std::hash<U32>()(p.second) << 1);
        }
    };
}

class SparseImage {
public:
    VkImage image = VK_NULL_HANDLE;
    VkImageView view = VK_NULL_HANDLE;

    VkExtent3D granularity = {};
    std::unordered_map<std::pair<U32, U32>, VkDeviceMemory> memoryMap;

    bool init(
            VulkanInfo* vkInfo,
            Vector<U32, 3> size,
            VkFormat format,
            VkImageUsageFlags usage
    );

    void bindMemory(U32 tileX, U32 tileY, VkDeviceMemory memory);
    void bindTile(U32 tileX, U32 tileY);
    void unbindTile(U32 tileX, U32 tileY);

    void shutdown();

private:
    VulkanInfo* m_vkInfo = nullptr;

};

