// src/ResourceManagement/RenderResources/SparseBuffer.cpp

#include "SparseImage.hpp"

#include "RenderEngine/VkUtils.hpp"

bool SparseImage::init(
        VulkanInfo* vkInfo,
        Vector<U32, 3> size,
        VkFormat format,
        VkImageUsageFlags usage
) {
    m_vkInfo = vkInfo;

    // Check device capabilities
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(m_vkInfo->physicalDevice, &features);
    assert(features.sparseBinding);

    // Get min tile size
    VkSparseImageMemoryRequirements sparseReq = {};
    uint32_t count = 1;
    vkGetImageSparseMemoryRequirements(m_vkInfo->device, image, &count, &sparseReq);
    granularity = sparseReq.formatProperties.imageGranularity;

    // Create VkImage
    VkImageCreateFlags flags = VK_IMAGE_CREATE_SPARSE_BINDING_BIT | VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT;
    U32 mipLevels = 1;
    U32 arrayLayers = 1;

    std::vector families = {vkInfo->graphicsQueueFamily, vkInfo->transferQueueFamily};

    VkImageCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = flags,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = size,
        .mipLevels = mipLevels,
        .arrayLayers = arrayLayers,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_CONCURRENT,
        .queueFamilyIndexCount = static_cast<U32>(families.size()),
        .pQueueFamilyIndices = families.data(),
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    VkResult result = vkCreateImage(m_vkInfo->device, &createInfo, nullptr, &image);
    if (!VkUtils::checkVkResult(result, "Failed to create sparse Image!")) {
        return false;
    }

    return true;
}

void SparseImage::bindMemory(U32 tileX, U32 tileY, VkDeviceMemory memory) {
    VkSparseImageMemoryBind bind{};
    bind.subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    bind.subresource.mipLevel = 0;
    bind.subresource.arrayLayer = 0;
    bind.offset = {
        .x = static_cast<int32_t>(tileX * granularity.width),
        .y = static_cast<int32_t>(tileY * granularity.height),
        .z = 0
    };
    bind.extent = granularity;
    bind.memory = memory;   // unbind it
    bind.memoryOffset = 0;
    bind.flags = 0;

    VkSparseImageMemoryBindInfo bindInfo{};
    bindInfo.image = image;
    bindInfo.bindCount = 1;
    bindInfo.pBinds = &bind;

    VkBindSparseInfo sparseInfo{};
    sparseInfo.sType = VK_STRUCTURE_TYPE_BIND_SPARSE_INFO;
    sparseInfo.imageBindCount = 1;
    sparseInfo.pImageBinds = &bindInfo;

    vkQueueBindSparse(m_vkInfo->graphicsQueue, 1, &sparseInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_vkInfo->graphicsQueue);   // wait for binding to complete
}

void SparseImage::bindTile(U32 tileX, U32 tileY) {
    assert(image != nullptr);

    VkMemoryRequirements memReq;
    vkGetImageMemoryRequirements(m_vkInfo->device, image, &memReq);

    // Align memory size to granularity
    VkMemoryAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = nullptr,
        .allocationSize = memReq.alignment,
        .memoryTypeIndex = VkUtils::findMemoryType(m_vkInfo, memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
    };

    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkResult result = vkAllocateMemory(m_vkInfo->device, &allocInfo, nullptr, &memory);
    if (!VkUtils::checkVkResult(result, "Failed to allocate sparse tile memory")) return;

    // Bind tile
    bindMemory(tileX, tileY, memory);

    // Store VkDeviceMemory
    memoryMap[{tileX, tileY}] = memory;
}

void SparseImage::unbindTile(U32 tileX, U32 tileY) {
    auto it = memoryMap.find({tileX, tileY});
    if (it == memoryMap.end()) return;

    VkDeviceMemory memory = it->second;

    bindMemory(tileX, tileY, nullptr);

    vkFreeMemory(m_vkInfo->device, memory, nullptr);
    memoryMap.erase(it);
}

void SparseImage::shutdown() {
    for (auto& [coord, mem] : memoryMap) {
        unbindTile(coord.first, coord.second);
        vkFreeMemory(m_vkInfo->device, mem, nullptr);
    }

    vkDestroyImage(m_vkInfo->device, image, nullptr);
    vkDestroyImageView(m_vkInfo->device, view, nullptr);
    memoryMap.clear();
}

