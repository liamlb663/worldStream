// src/ResourceManagement/MaterialManager.cpp

#include "MaterialManager.hpp"

#include "RenderEngine/RenderObjects/Materials.hpp"
#include "ResourceManagement/MaterialManagerUtils/yamlParsers.hpp"

#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

bool MaterialManager::initialize(VulkanInfo* vkInfo) {
    m_vkInfo = vkInfo;
    return true;
}

void MaterialManager::shutdown() {
    // Clear all images
    for (auto& pair : m_materialInfos) {
        auto& refCount = pair.second;
        if (refCount.references > 0) {
            destroyMaterialInfo(&refCount.value);
        }
    }
    m_materialInfos.clear();
}

DescriptorLayoutInfo MaterialManager::getLayout(std::string path) {
    auto it = m_descriptorLayouts.find(path);
    if (it != m_descriptorLayouts.end()) {
        it->second.references++;
        return it->second.value;
    }

    fs::path fullPath = resourceBasePath / path;

    if (!fs::exists(fullPath)) {
        spdlog::error("Material Descriptor not found: {}", fullPath.string());
    }

    // Get Material Info
    YAML::Node yaml = YAML::LoadFile(fullPath);
    DescriptorLayoutInfo layout = MaterialManagerUtils::yamlToLayout(yaml, m_vkInfo->device).value(); // HACK: unwrap

    m_descriptorLayouts[path] = RefCount<DescriptorLayoutInfo>{
        .value = layout,
        .references = 1,
    };

    return m_descriptorLayouts[path].value;
}

MaterialInfo* MaterialManager::getInfo(std::string path) {
    auto it = m_materialInfos.find(path);
    if (it != m_materialInfos.end()) {
        it->second.references++;
        return &it->second.value;
    }

    fs::path materialFolder = resourceBasePath / path;
    fs::path fullPath = materialFolder / "pipeline.yaml";

    if (!fs::exists(fullPath)) {
        spdlog::error("Material Descriptor not found: {}", fullPath.string());
    }

    // Get Material Info
    YAML::Node yaml = YAML::LoadFile(fullPath);
    MaterialInfo matInfo = MaterialManagerUtils::yamlToInfo(this, yaml, resourceBasePath, path, m_vkInfo->device).value();  // HACK: unwrap

    m_materialInfos[path] = RefCount<MaterialInfo>{
        .value = matInfo,
        .references = 1,
    };

    return &m_materialInfos[path].value;
}

void MaterialManager::destroyMaterialInfo(MaterialInfo* info) {
    vkDestroyPipeline(m_vkInfo->device, info->pipeline, nullptr);
    vkDestroyPipelineLayout(m_vkInfo->device, info->pipelineLayout, nullptr);

    for (DescriptorLayoutInfo layout : info->descriptorLayouts) {
        this->dropLayout(&layout);
    }
}

bool compareMaterialInfo(MaterialInfo a, MaterialInfo b) {
   return a.pipeline == b.pipeline &&
          a.pipelineLayout == b.pipelineLayout &&
          a.type == b.type &&
          a.descriptorLayouts == b.descriptorLayouts;
}

void MaterialManager::dropMaterialInfo(MaterialInfo* info) {
    for (auto it = m_materialInfos.begin(); it != m_materialInfos.end(); ++it) {
        auto sharedResource = it->second.value;
        if (compareMaterialInfo(sharedResource, *info)) {
            // Decrement reference count
            it->second.references--;

            if (it->second.references <= 0) {
                // If reference count is zero, apply custom shutdown logic
                destroyMaterialInfo(info);
                m_materialInfos.erase(it);
            }
            return;
        }
    }

    spdlog::error("MaterialInfo not found for dropping!");
}

void MaterialManager::destroyDescriptorLayoutInfo(DescriptorLayoutInfo* info) {
    vkDestroyDescriptorSetLayout(m_vkInfo->device, info->layout, nullptr);
    info->bindings.clear();
}

void MaterialManager::dropLayout(DescriptorLayoutInfo* layout) {
    for (auto it = m_descriptorLayouts.begin(); it != m_descriptorLayouts.end(); ++it) {
        auto sharedResource = it->second.value;
        if (sharedResource == *layout) {
            // Decrement reference count
            it->second.references--;

            if (it->second.references <= 0) {
                // If reference count is zero, apply custom shutdown logic
                destroyDescriptorLayoutInfo(layout);
                m_descriptorLayouts.erase(it);
            }
            return;
        }
    }

    spdlog::error("Layout not found for dropping!");
}

MaterialData MaterialManager::getData(std::string path, Buffer buffer, DescriptorBuffer* descriptor) {
    MaterialInfo* materialInfo = getInfo(path);

    std::vector<DescriptorSetData> descriptorSets = {};

    for (Size i = 0; i < materialInfo->descriptorLayouts.size(); i++) {
        std::vector<DescriptorBindingData> bindingDatas;
        for (DescriptorBindingInfo info : materialInfo->descriptorLayouts[i].bindings) {
            U32 index = descriptor->allocateBufferDescriptor(buffer, info.size);

            DescriptorBindingData bindingData = {
                .binding = info.binding,
                .descriptorIndex = index,
            };

            bindingDatas.push_back(bindingData);
        }

        DescriptorSetData info = {
            .buffer = descriptor,
            .set = static_cast<U32>(i),
            .bindings = bindingDatas,
        };

        descriptorSets.push_back(info);
    }

    MaterialData data = {
        .pipeline = materialInfo,
        .descriptorSets = descriptorSets,
    };

    return data;
}

void MaterialManager::dropMaterialData(MaterialData* data) {
    // HACK: not now ^
    (void) data;
}


