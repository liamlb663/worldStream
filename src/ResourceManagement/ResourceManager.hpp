// src/ResourceManagement/ResourceManager.hpp

#pragma once

#include "RenderEngine/VulkanInfo.hpp"

#include <memory>

class ResourceManager {
public:
    bool initialize(std::shared_ptr<VulkanInfo> vkInfo);
    void shutdown();

private:
    std::shared_ptr<VulkanInfo> m_vkInfo;

};

