// src/ResourceManagement/ResourceManager.hpp

#pragma once

#include "RenderEngine/CommandSubmitter.hpp"
#include "RenderEngine/VulkanInfo.hpp"

#include "RenderResources/Buffer.hpp"
#include "RenderResources/Image.hpp"
#include "ResourceManagement/MaterialManager.hpp"
#include "ResourceManagement/RenderResources/DescriptorBuffer.hpp"

#include <memory>
#include <unordered_map>
#include <filesystem>
#include <expected>

namespace fs = std::filesystem;

class ResourceManager {
public:
    bool initialize(VulkanInfo* vkInfo, std::shared_ptr<CommandSubmitter> submitter);
    void shutdown();

    // Images
    std::shared_ptr<Image> loadImage(std::string path);
    void copyToImage(void* data, Size size, std::shared_ptr<Image> image);
    void dropImage(std::shared_ptr<Image> image);

    // Buffers
    std::expected<Buffer, U32> createStagingBuffer(Size size);
    std::expected<Buffer, U32> createIndexBuffer(Size size);
    std::expected<Buffer, U32> createVertexBuffer(Size size);
    std::expected<Buffer, U32> createUniformBuffer(Size size);
    std::expected<Buffer, U32> createStorageBuffer(Size size);

    std::expected<Buffer, U32> createBuffer(
            Size size,
            VkBufferUsageFlags bufferUsage,
            VmaMemoryUsage memoryUsage,
            VmaAllocationCreateFlags allocFlags
    );

    std::expected<DescriptorBuffer, U32> createDescriptorBuffer(
            Size size
    );

    MaterialManager* getMaterialManager() { return &m_materialManager; };

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

    std::unordered_map<std::string, RefCount<std::shared_ptr<Image>>> m_images;
};

