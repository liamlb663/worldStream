// src/ResourceManagement/MaterialManager.hpp

#pragma once

#include "RenderEngine/VulkanInfo.hpp"
#include "RenderEngine/RenderObjects/Materials.hpp"
#include "ResourceManagement/RenderResources/DescriptorPool.hpp"

#include <vulkan/vulkan.h>

#include <unordered_map>
#include <filesystem>

namespace fs = std::filesystem;

class MaterialManager {
public:
    bool initialize(VulkanInfo* vkInfo);
    void shutdown();

    DescriptorSetInfo getLayout(std::string path);
    void dropLayout(DescriptorSetInfo* layout);

    MaterialInfo* getInfo(std::string path);
    void dropMaterialInfo(MaterialInfo* info);

    MaterialData getData(std::string path, DescriptorPool* descriptor);
    void dropMaterialData(MaterialData* data);

private:
    template <typename ResourceType>
    struct RefCount {
        ResourceType value;
        Size references;
    };

    VulkanInfo* m_vkInfo;

    fs::path resourceBasePath = "assets/materials";

    std::unordered_map<std::string, RefCount<MaterialInfo>> m_materialInfos;
    std::unordered_map<std::string, RefCount<DescriptorSetInfo>> m_descriptorLayouts;

    void destroyMaterialInfo(MaterialInfo* info);
    void destroyDescriptorLayoutInfo(DescriptorSetInfo* info);
};

