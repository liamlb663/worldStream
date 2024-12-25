// src/RenderEngine/RenderResources/Image.hpp

#include "Image.hpp"

bool Image::init(
        std::shared_ptr<VulkanInfo> vkInfo,
        const Vector<U32, 3>& imageSize,
        VkFormat imageFormat,
        VkImageUsageFlags usage,
        VkImageAspectFlags aspect
) {

}

void Image::shutdown() {
    vkDestroyImageView(m_vkInfo->device, view, nullptr);
    vmaDestroyImage(m_vkInfo->allocator, image, allocation);
}

