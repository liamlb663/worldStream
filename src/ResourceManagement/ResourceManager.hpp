// src/ResourceManagement/ResourceManager.hpp

#pragma once

#include "RenderEngine/CommandSubmitter.hpp"
#include "RenderEngine/VulkanInfo.hpp"

#include "RenderResources/Buffer.hpp"
#include "RenderResources/Image.hpp"

#include <memory>
#include <unordered_map>
#include <filesystem>

namespace fs = std::filesystem;

class ResourceManager {
public:
    bool initialize(std::shared_ptr<VulkanInfo> vkInfo, std::shared_ptr<CommandSubmitter> submitter);
    void shutdown();

    // Images
    std::shared_ptr<Image> loadImage(std::string path);
    void copyToImage(void* data, Size size, std::shared_ptr<Image> image);
    void dropImage(std::shared_ptr<Image> image);

    // Buffers
    std::shared_ptr<Buffer> createStagingBuffer(Size size);
    std::shared_ptr<Buffer> createVertexBuffer(Size size);
    std::shared_ptr<Buffer> createUniformBuffer(Size size);
    std::shared_ptr<Buffer> createStorageBuffer(Size size);

    std::shared_ptr<Buffer> createBuffer(
            Size size,
            VkBufferUsageFlags bufferUsage,
            VmaMemoryUsage memoryUsage,
            VmaAllocationCreateFlags allocFlags
    );

private:
    template <typename ResourceType>
    struct RefCount {
        ResourceType value;
        Size references;
    };

    std::shared_ptr<VulkanInfo> m_vkInfo;
    std::shared_ptr<CommandSubmitter> m_submitter;

    fs::path resourceBasePath = "assets";

    std::unordered_map<std::string, RefCount<std::shared_ptr<Image>>> m_images;
};

