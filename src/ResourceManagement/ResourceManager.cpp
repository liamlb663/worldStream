// src/ResourceManagement/ResourceManager.cpp

#include "ResourceManager.hpp"

bool ResourceManager::initialize(std::shared_ptr<VulkanInfo> vkInfo) {
    m_vkInfo = vkInfo;

    return true;
}

void ResourceManager::shutdown() {

}

