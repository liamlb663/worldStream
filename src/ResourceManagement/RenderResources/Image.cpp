// src/ResourceManagement/RenderResources/Image.hpp

#include "Image.hpp"

#include "RenderEngine/VkUtils.hpp"
#include "RenderEngine/VulkanInfo.hpp"

bool Image::init(
        std::shared_ptr<VulkanInfo> vkInfo,
        const Vector<U32, 3>& imageSize,
        VkFormat imageFormat,
        VkImageUsageFlags usage
) {
    size = imageSize;
    format = imageFormat;

    std::vector families = {vkInfo->graphicsQueueFamily, vkInfo->transferQueueFamily};

    VkImageCreateInfo imageInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = size,
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,   // TODO: Anti-Aliasing
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
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

	// if the format is a depth format, we will need to
    // have it use the correct aspect flag
	VkImageAspectFlags aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
	if (format == VK_FORMAT_D32_SFLOAT) {
		aspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT;
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
            .aspectMask = aspectFlag,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = imageInfo.mipLevels,
        },
    };

	result = vkCreateImageView(vkInfo->device, &viewInfo, nullptr, &view);
    if (!VkUtils::checkVkResult(result, "Could not create the image view")) {
        return false;
    }

    return true;
}

void Image::shutdown() {
    vkDestroyImageView(m_vkInfo->device, view, nullptr);
    vmaDestroyImage(m_vkInfo->allocator, image, allocation);
}

