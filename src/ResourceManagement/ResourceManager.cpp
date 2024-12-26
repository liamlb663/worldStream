// src/ResourceManagement/ResourceManager.cpp

#include "ResourceManager.hpp"

#include "spdlog/spdlog.h"

#include <memory>

bool ResourceManager::initialize(std::shared_ptr<VulkanInfo> vkInfo) {
    m_vkInfo = vkInfo;

    return true;
}

void ResourceManager::shutdown() {

}

std::shared_ptr<Image> ResourceManager::loadImage(std::string path) {
    auto it = m_images.find(path);
    if (it != m_images.end()) {
        return it->second;
    }

    fs::path fullPath = resourceBasePath / path;

    if (!fs::exists(fullPath)) {
        spdlog::error("Resource not found: {}", fullPath.string());
    }

    // TODO: Using fs and stb, load the data and then create the Image

    return nullptr;
}
