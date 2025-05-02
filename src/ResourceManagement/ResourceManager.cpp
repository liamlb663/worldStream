// src/ResourceManagement/ResourceManager.cpp

#include "ResourceManager.hpp"

#include "RenderResources/Buffer.hpp"
#include "RenderResources/Image.hpp"
#include "spdlog/spdlog.h"

#include <cassert>
#include <vulkan/vulkan.h>
#include <stb_image.h>

#include <cstring>

bool ResourceManager::initialize(VulkanInfo* vkInfo, std::shared_ptr<CommandSubmitter> submitter) {
    m_vkInfo = vkInfo;
    m_submitter = submitter;

    if (!m_materialManager.initialize(vkInfo))
        return false;

    return true;
}

void ResourceManager::shutdown() {
    m_materialManager.shutdown();

    // Clear all images
    for (auto& pair : m_images) {
        auto& refCount = pair.second;
        if (refCount.references > 0) {
            refCount.value.shutdown();
        }
    }
    m_images.clear();

    spdlog::info("ResourceManager shutdown completed");
}

VulkanInfo* ResourceManager::getVkInfo() {
    return m_vkInfo;
}

std::expected<Image, std::string> ResourceManager::createImage(
        Vector<U32, 2> size,
        VkFormat format,
        VkImageUsageFlags usage,
        ImageType type,
        std::string name
) {
    Image image;

    VkImageCreateFlags createFlags = 0;
    VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
    U32 layers = 1;

    switch (type) {
        case ImageType::Texture2D:
            viewType = VK_IMAGE_VIEW_TYPE_2D;
            break;
        case ImageType::CubeMap:
            viewType = VK_IMAGE_VIEW_TYPE_CUBE;
            createFlags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
            layers = 6;
            break;
    }

    if (!image.init(m_vkInfo, size, format, usage, layers, createFlags, viewType, name)) {
        return std::unexpected("Failed to initialize image");
    }

    return image;
}

Image* ResourceManager::loadImage(std::string path, const LoadImageConfig& config) {
    auto it = m_images.find(path);
    if (it != m_images.end()) {
        it->second.references++;
        return &it->second.value;
    }

    fs::path fullPath = resourceBasePath / path;
    if (!fs::exists(fullPath)) {
        spdlog::error("Resource not found: {}", fullPath.string());
        return nullptr;
    }

    int width, height, channels;
    U8* data = stbi_load(fullPath.c_str(), &width, &height, &channels, 4);
    if (!data) {
        spdlog::error("Failed to load image: {}", stbi_failure_reason());
        return nullptr;
    }

    auto result = createImage(
        {static_cast<U32>(width), static_cast<U32>(height)},
        config.format,
        config.usage,
        config.type,
        path
    );

    if (!result.has_value()) {
        spdlog::error("Image creation failed: {}", result.error());
        stbi_image_free(data);
        return nullptr;
    }

    Image img = result.value();
    copyToImage(data, width * height * 4, &img);
    stbi_image_free(data);

    m_images[path] = RefCount<Image>{
        .value = img,
        .references = 1,
    };

    return &m_images[path].value;
}

void ResourceManager::copyToImage(void* data, Size size, Image* image) {
    // Create and fill buffer
    std::expected<Buffer, U32> stagingReturn = createStagingBuffer(size + 1);

    if (!stagingReturn.has_value())
        return;

    Buffer staging = stagingReturn.value();

    memcpy(staging.info.pMappedData, data, size);

    VkCommandBuffer cmd = m_submitter->transferSubmitStart();

    m_submitter->transitionImage(cmd, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
        static_cast<U32>(image->size.value.x),
        static_cast<U32>(image->size.value.y),
        (U32)1
    };

    vkCmdCopyBufferToImage(cmd, staging.buffer, image->image, image->layout, 1, &region);

    m_submitter->transitionImage(cmd, image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, true);

    m_submitter->transferSubmitEnd(cmd);

    staging.shutdown();
}

void ResourceManager::dropImage(Image* image) {
    for (auto it = m_images.begin(); it != m_images.end(); ++it) {
        auto sharedResource = it->second.value;
        if (sharedResource.image == image->image) {
            // Decrement reference count
            it->second.references--;

            if (it->second.references <= 0) {
                // If reference count is zero, apply custom shutdown logic
                image->shutdown();
                m_images.erase(it);
            }
            return;
        }
    }

    spdlog::error("Image not found for dropping!");
}

std::expected<Buffer, U32> ResourceManager::createStagingBuffer(Size size) {
    VkBufferUsageFlags bufferUsage =
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;

    VmaAllocationCreateFlags allocFlags = VMA_ALLOCATION_CREATE_MAPPED_BIT |
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

    return createBuffer(size, bufferUsage, memoryUsage, allocFlags);
}

std::expected<Buffer, U32> ResourceManager::createIndexBuffer(Size size) {
    VkBufferUsageFlags usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                               VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;

    return createBuffer(size, usage, memoryUsage, 0);
}

std::expected<Buffer, U32> ResourceManager::createVertexBuffer(Size size) {
    VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                               VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;

    return createBuffer(size, usage, memoryUsage, 0);
}

std::expected<Buffer, U32> ResourceManager::createUniformBuffer(Size size) {
    VkBufferUsageFlags usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT |
                               VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    VmaAllocationCreateFlags allocFlags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

    return createBuffer(size, usage, memoryUsage, allocFlags);
}

std::expected<Buffer, U32> ResourceManager::createStorageBuffer(Size size) {
    VkBufferUsageFlags usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                               VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;

    return createBuffer(size, usage, memoryUsage, 0);
}

std::expected<Buffer, U32> ResourceManager::createBuffer(
        Size size,
        VkBufferUsageFlags usage,
        VmaMemoryUsage memoryUsage,
        VmaAllocationCreateFlags allocFlags
) {
    Buffer buff;
    if (!buff.init(m_vkInfo, size, usage, memoryUsage, allocFlags)) {
        return std::unexpected(2);  // TODO: Fix with expected
    }
    return buff;
}

void ResourceManager::copyToBuffer(const Buffer& src, const Buffer& dst, Size size) {
    assert(m_submitter && "CommandSubmitter must be initialized!");

    m_submitter->transferSubmit([&](VkCommandBuffer cmd) {
        VkBufferCopy copyRegion = {
            .srcOffset = 0,
            .dstOffset = 0,
            .size = size
        };

        vkCmdCopyBuffer(cmd, src.buffer, dst.buffer, 1, &copyRegion);
    });
}

std::expected<DescriptorPool, U32> ResourceManager::createDescriptorPool(
        U32 setCount, std::span<DescriptorPool::PoolSizeRatio> poolRatios
) {
    DescriptorPool descriptor;

    if (!descriptor.init(m_vkInfo, setCount, poolRatios)) {
        spdlog::error("Failed to Create DescriptorBuffer from ResourceManager");
        return std::unexpected(1);
    }
    return descriptor;
}
