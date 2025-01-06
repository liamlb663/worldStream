// src/ResourceManagement/ResourceManager.cpp

#include "ResourceManager.hpp"

#include "RenderResources/Buffer.hpp"
#include "RenderResources/Image.hpp"
#include "spdlog/spdlog.h"

#include <vulkan/vulkan.h>
#include <stb_image.h>

#include <cstring>
#include <memory>

bool ResourceManager::initialize(std::shared_ptr<VulkanInfo> vkInfo, std::shared_ptr<CommandSubmitter> submitter) {
    m_vkInfo = vkInfo;
    m_submitter = submitter;

    return true;
}

void ResourceManager::shutdown() {
    // Clear all images
    for (auto& pair : m_images) {
        auto& refCount = pair.second;
        if (refCount.references > 0) {
            refCount.value->shutdown();
        }
    }
    m_images.clear();

    spdlog::info("ResourceManager shutdown completed");
}

std::shared_ptr<Image> ResourceManager::loadImage(std::string path) {
    auto it = m_images.find(path);
    if (it != m_images.end()) {
        it->second.references++;
        return it->second.value;
    }

    fs::path fullPath = resourceBasePath / path;

    if (!fs::exists(fullPath)) {
        spdlog::error("Resource not found: {}", fullPath.string());
    }

    int width, height, channels;
    U8* data = stbi_load(fullPath.c_str(), &width, &height, &channels, 0);
    if (!data) {
        spdlog::error("Failed to load image: {}", stbi_failure_reason());
        return nullptr;
    }

    VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
    VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    std::shared_ptr<Image> img = std::make_shared<Image>();
    img->init(m_vkInfo, Vector<U32, 2>(width, height), format, usage, path);

    copyToImage(data, width * height * 4, img);
    stbi_image_free(data);

    m_images[path] = RefCount<std::shared_ptr<Image>>{
        .value = img,
        .references = 1,
    };

    return img;
}

void ResourceManager::copyToImage(void* data, Size size, std::shared_ptr<Image> image) {
    // Create and fill buffer
    std::shared_ptr<Buffer> staging = createStagingBuffer(size + 1);
    memcpy(staging->info.pMappedData, data, size);

    m_submitter->transferSubmit([this, staging, image](VkCommandBuffer cmd){
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

        vkCmdCopyBufferToImage(cmd, staging->buffer, image->image, image->layout, 1, &region);
    });

    staging->shutdown();
}

void ResourceManager::dropImage(std::shared_ptr<Image> image) {
    for (auto it = m_images.begin(); it != m_images.end(); ++it) {
        auto sharedResource = it->second.value;
        if (sharedResource == image) {
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

std::shared_ptr<Buffer> ResourceManager::createStagingBuffer(Size size) {
    VkBufferUsageFlags bufferUsage =
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;

    VmaAllocationCreateFlags allocFlags = VMA_ALLOCATION_CREATE_MAPPED_BIT |
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

    return createBuffer(size, bufferUsage, memoryUsage, allocFlags);
}

std::shared_ptr<Buffer> ResourceManager::createVertexBuffer(Size size) {
    VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                               VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;

    return createBuffer(size, usage, memoryUsage, 0);
}

std::shared_ptr<Buffer> ResourceManager::createUniformBuffer(Size size) {
    VkBufferUsageFlags usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    VmaAllocationCreateFlags allocFlags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

    return createBuffer(size, usage, memoryUsage, allocFlags);
}

std::shared_ptr<Buffer> ResourceManager::createStorageBuffer(Size size) {
    VkBufferUsageFlags usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                               VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;

    return createBuffer(size, usage, memoryUsage, 0);
}

std::shared_ptr<Buffer> ResourceManager::createBuffer(
        Size size,
        VkBufferUsageFlags usage,
        VmaMemoryUsage memoryUsage,
        VmaAllocationCreateFlags allocFlags
) {
    std::shared_ptr<Buffer> buff = std::make_shared<Buffer>();
    if (!buff->init(m_vkInfo, size, usage, memoryUsage, allocFlags)) {
        return nullptr;
    }
    return buff;
}

