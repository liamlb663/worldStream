// src/ResourceManagement/MaterialManager.hpp

#pragma once

#include "RenderEngine/VulkanInfo.hpp"
#include "RenderEngine/RenderObjects/Materials.hpp"

#include <vulkan/vulkan.h>

#include <memory>
#include <unordered_map>
#include <filesystem>

namespace fs = std::filesystem;

class MaterialManager {
public:
    bool initialize(std::shared_ptr<VulkanInfo> vkInfo);
    void shutdown();

    VkDescriptorSetLayout getLayout(std::string path);
    MaterialInfo* getInfo(std::string path);

private:
    template <typename ResourceType>
    struct RefCount {
        ResourceType value;
        Size references;
    };

    std::shared_ptr<VulkanInfo> m_vkInfo;

    fs::path resourceBasePath = "assets/materials";
    fs::path pipelinesPath = "pipelines";
    fs::path layoutsPath = "descriptors";

    std::unordered_map<std::string, RefCount<MaterialInfo>> m_materialInfos;
    std::unordered_map<std::string, RefCount<VkDescriptorSetLayout>> m_descriptorLayouts;
};

