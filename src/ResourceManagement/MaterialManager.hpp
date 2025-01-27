// src/ResourceManagement/MaterialManager.hpp

#pragma once

#include "RenderEngine/VulkanInfo.hpp"
#include "RenderEngine/RenderObjects/Materials.hpp"

#include <memory>
#include <unordered_map>
#include <filesystem>

namespace fs = std::filesystem;

class MaterialManager {
public:
    bool initialize(std::shared_ptr<VulkanInfo> vkInfo);
    void shutdown();

    MaterialInfo* getInfo(std::string path);

private:
    template <typename ResourceType>
    struct RefCount {
        ResourceType value;
        Size references;
    };

    std::shared_ptr<VulkanInfo> m_vkInfo;

    fs::path resourceBasePath = "assets/materials";

    std::unordered_map<std::string, RefCount<MaterialInfo>> m_materialInfos;
};

