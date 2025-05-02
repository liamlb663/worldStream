// src/ResourceManagement/RenderResources/Image.hpp

#include "Image.hpp"

#include "RenderEngine/Debug.hpp"
#include "RenderEngine/VkUtils.hpp"
#include "RenderEngine/VulkanInfo.hpp"
#include "spdlog/spdlog.h"

#include <fmt/core.h>

bool Image::init(
        VulkanInfo* vkInfo,
        const Vector<U32, 2>& imageSize,
        VkFormat imageFormat,
        VkImageUsageFlags usage,
        U32 layers,
        VkImageCreateFlags flags,
        VkImageViewType viewType,
        std::string name
) {
    m_vkInfo = vkInfo;
    size = imageSize;
    Vector<U32, 3> size3D = {size.value.x, size.value.y, 1};
    format = imageFormat;
    this->layers = layers;
    U32 mipLevels = 1;

    std::vector families = {vkInfo->graphicsQueueFamily, vkInfo->transferQueueFamily};

    if (viewType == VK_IMAGE_VIEW_TYPE_CUBE && size.value.x != size.value.y) {
        spdlog::warn("Cube maps should be square (got {}x{})", size.value.x, size.value.y);
    }

    if ((viewType == VK_IMAGE_VIEW_TYPE_CUBE || viewType == VK_IMAGE_VIEW_TYPE_CUBE_ARRAY) &&
        !(flags & VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT)) {
        spdlog::warn("Cube map view type requires VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT to be set in flags.");
    }

    VkImageCreateInfo imageInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = flags,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = size3D,
        .mipLevels = mipLevels,
        .arrayLayers = layers,
        .samples = VK_SAMPLE_COUNT_1_BIT,   // TODO: Anti-Aliasing
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_CONCURRENT,
        // Shift to using Image Ownership Transfer with exclusing sharing for better performance
        .queueFamilyIndexCount = static_cast<U32>(families.size()),
        .pQueueFamilyIndices = families.data(),
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    VmaAllocationCreateInfo allocInfo = {
        .flags = 0,
        .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
        .requiredFlags = 0,
        .preferredFlags = 0,
        .memoryTypeBits = 0,
        .pool = nullptr,
        .pUserData = nullptr,
        .priority = 0,
    };

    VkResult result = vmaCreateImage(
            vkInfo->allocator,
            &imageInfo,
            &allocInfo,
            &image,
            &allocation,
            nullptr);

    if (!VkUtils::checkVkResult(result, "Could not create the image")) {
        return false;
    }

    Debug::SetObjectName(vkInfo->device, (U64)image, VK_OBJECT_TYPE_IMAGE, fmt::format("{}'s image", name).c_str());

    // if the format is a depth format, we will need to
    // have it use the correct aspect flag
    VkImageAspectFlags aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
    if (format == VK_FORMAT_D32_SFLOAT || format == VK_FORMAT_D16_UNORM) {
        aspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT;
    } else if (format == VK_FORMAT_D24_UNORM_S8_UINT || format == VK_FORMAT_D32_SFLOAT_S8_UINT) {
        aspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    VkImageViewCreateInfo viewInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .image = image,
        .viewType = viewType,
        .format = format,

        .components = {
            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VK_COMPONENT_SWIZZLE_IDENTITY,
        },

        .subresourceRange = {
            .aspectMask = aspectFlag,
            .baseMipLevel = 0,
            .levelCount = mipLevels,
            .baseArrayLayer = 0,
            .layerCount = layers,
        },
    };

    if ((viewType == VK_IMAGE_VIEW_TYPE_CUBE || viewType == VK_IMAGE_VIEW_TYPE_CUBE_ARRAY) &&
        (layers % 6 != 0)) {
        spdlog::error("Cubemaps require layers to be a multiple of 6 (got {}).", layers);
        return false;
    }

    result = vkCreateImageView(vkInfo->device, &viewInfo, nullptr, &view);
    if (!VkUtils::checkVkResult(result, "Could not create the image view")) {
        return false;
    }

    Debug::SetObjectName(vkInfo->device, (U64)view, VK_OBJECT_TYPE_IMAGE_VIEW, fmt::format("{}'s image view", name).c_str());

    return true;
}

VkImageView Image::createLayerView(
    VulkanInfo* vkInfo,
    U32 layerIndex,
    const std::string& debugName
) const {
    if (image == VK_NULL_HANDLE) {
        spdlog::error("Attempted to create layer view on uninitialized image.");
        return VK_NULL_HANDLE;
    }

    if (layerIndex >= this->layers) {
        spdlog::error("Layer index {} out of bounds (max {}).", layerIndex, this->layers - 1);
        return VK_NULL_HANDLE;
    }

    VkImageViewCreateInfo viewInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .image = image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,

        .components = {
            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VK_COMPONENT_SWIZZLE_IDENTITY,
        },

        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = layerIndex,
            .layerCount = 1,
        }
    };

    VkImageView view;
    VkResult result = vkCreateImageView(vkInfo->device, &viewInfo, nullptr, &view);
    if (!VkUtils::checkVkResult(result,
            fmt::format("Failed to create layer view at layer {}", layerIndex))) {
        return VK_NULL_HANDLE;
    }

    if (!debugName.empty()) {
        Debug::SetObjectName(vkInfo->device, (uint64_t)view, VK_OBJECT_TYPE_IMAGE_VIEW, debugName.c_str());
    }

    return view;
}

void Image::shutdown() {
    if (image == VK_NULL_HANDLE) {
        spdlog::warn("Attempted to destroy image a second time!");
        return;
    }

    vmaDestroyImage(m_vkInfo->allocator, image, allocation);
    vkDestroyImageView(m_vkInfo->device, view, nullptr);

    image = VK_NULL_HANDLE;
    view = VK_NULL_HANDLE;
    allocation = VK_NULL_HANDLE;
    size = {0,0};
    format = VK_FORMAT_UNDEFINED;
    layout = VK_IMAGE_LAYOUT_UNDEFINED;
    layers = 0;
}

