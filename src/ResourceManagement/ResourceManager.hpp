// src/ResourceManagement/ResourceManager.hpp

#pragma once

#include "RenderEngine/VulkanInfo.hpp"

#include "RenderResources/Buffer.hpp"
#include "RenderResources/Image.hpp"

#include <memory>
#include <unordered_map>
#include <filesystem>

namespace fs = std::filesystem;

class ResourceManager {
public:
    bool initialize(std::shared_ptr<VulkanInfo> vkInfo);
    void shutdown();

    std::shared_ptr<Image> loadImage(std::string path);
    std::shared_ptr<Buffer> loadBuffer();

private:
    std::shared_ptr<VulkanInfo> m_vkInfo;

    fs::path resourceBasePath = "/resources"; // Base path for resources
                                              //
    std::unordered_map<std::string, std::shared_ptr<Image>> m_images;
    std::unordered_map<std::string, std::shared_ptr<Buffer>> m_buffers;

};

