// src/ResourceManagement/ResourceManager.hpp

#pragma once

#include "RenderEngine/CommandSubmitter.hpp"
#include "RenderEngine/VulkanInfo.hpp"

#include "RenderResources/Buffer.hpp"
#include "RenderResources/Image.hpp"
#include "ResourceManagement/MaterialManager.hpp"
#include "ResourceManagement/RenderResources/DescriptorPool.hpp"
#include "ResourceManagement/RenderResources/Sampler.hpp"

#include <memory>
#include <unordered_map>
#include <filesystem>
#include <expected>

namespace fs = std::filesystem;

enum class ImageType {
    Texture2D,
    CubeMap,
};

struct LoadImageConfig {
    ImageType type;
    VkFormat format;
    VkImageUsageFlags usage;
};

class ResourceManager {
public:
    bool initialize(VulkanInfo* vkInfo, std::shared_ptr<CommandSubmitter> submitter);
    void shutdown();

    VulkanInfo* getVkInfo();

    // Images
    std::expected<Image, std::string> createImage(
            Vector<U32, 2> size,
            VkFormat format,
            VkImageUsageFlags usage,
            ImageType type,
            std::string name
    );
    Image* loadImage(std::string path, const LoadImageConfig& config);
    void copyToImage(void* data, Size size, Image* image);
    void dropImage(Image* image);

    // Buffers
    std::expected<Buffer, U32> createStagingBuffer(Size size, std::string name);
    std::expected<Buffer, U32> createIndexBuffer  (Size size, std::string name);
    std::expected<Buffer, U32> createVertexBuffer (Size size, std::string name);
    std::expected<Buffer, U32> createUniformBuffer(Size size, std::string name);
    std::expected<Buffer, U32> createStorageBuffer(Size size, std::string name);

    std::expected<Buffer, U32> createBuffer(
            Size size,
            VkBufferUsageFlags bufferUsage,
            VmaMemoryUsage memoryUsage,
            VmaAllocationCreateFlags allocFlags,
            std::string name
    );

    void copyToBuffer(const Buffer& src, const Buffer& dst, Size size);

    std::expected<DescriptorPool, U32> createDescriptorPool(
            U32 setCount, std::span<DescriptorPool::PoolSizeRatio> poolRatios
    );

    MaterialManager* getMaterialManager() { return &m_materialManager; };
    SamplerBuilder getSamplerBuilder() { return SamplerBuilder(m_vkInfo); };

private:
    template <typename ResourceType>
    struct RefCount {
        ResourceType value;
        Size references;
    };

    VulkanInfo* m_vkInfo;
    std::shared_ptr<CommandSubmitter> m_submitter;
    MaterialManager m_materialManager;

    fs::path resourceBasePath = "assets";

    std::unordered_map<std::string, RefCount<Image>> m_images;
};

