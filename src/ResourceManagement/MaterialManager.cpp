// src/ResourceManagement/MaterialManager.cpp

#include "MaterialManager.hpp"

#include "RenderEngine/RenderObjects/Materials.hpp"
#include "ResourceManagement/MaterialManagerUtils/yamlParsers.hpp"
#include "ResourceManagement/RenderResources/DescriptorBuffer.hpp"

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

DescriptorSetInfo MaterialManager::getLayout(std::string path) {
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
    DescriptorSetInfo layout = MaterialManagerUtils::yamlToLayout(yaml, m_vkInfo->device).value();

    m_descriptorLayouts[path] = RefCount<DescriptorSetInfo>{
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
    MaterialInfo matInfo = MaterialManagerUtils::yamlToInfo(this, yaml, resourceBasePath, path, m_vkInfo->device).value();

    m_materialInfos[path] = RefCount<MaterialInfo>{
        .value = matInfo,
        .references = 1,
    };

    return &m_materialInfos[path].value;
}

void MaterialManager::destroyMaterialInfo(MaterialInfo* info) {
    vkDestroyPipeline(m_vkInfo->device, info->pipeline, nullptr);
    vkDestroyPipelineLayout(m_vkInfo->device, info->pipelineLayout, nullptr);

    for (DescriptorSetInfo set : info->descriptorSets) {
        this->dropLayout(&set);
    }
}

bool compareMaterialInfo(MaterialInfo a, MaterialInfo b) {
   return a.pipeline == b.pipeline &&
          a.pipelineLayout == b.pipelineLayout &&
          a.type == b.type &&
          a.descriptorSets == b.descriptorSets;
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

void MaterialManager::destroyDescriptorLayoutInfo(DescriptorSetInfo* info) {
    vkDestroyDescriptorSetLayout(m_vkInfo->device, info->layout, nullptr);
    info->bindings.clear();
}

void MaterialManager::dropLayout(DescriptorSetInfo* layout) {
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

MaterialData MaterialManager::getData(std::string path, DescriptorBuffer* descriptor) {
    MaterialInfo* materialInfo = getInfo(path);

    std::vector<DescriptorSetData> descriptorSets = {};

    for (Size i = 0; i < materialInfo->descriptorSets.size(); i++) {
        std::vector<U32> bindingDatas;

        // TODO: Fix descriptor buffer
        U32 index = descriptor->allocateSlot(&materialInfo->descriptorSets[i]);

        for (DescriptorBindingInfo info : materialInfo->descriptorSets[i].bindings) {
            bindingDatas.push_back(info.binding);
        }

        DescriptorSetData info = {
            .buffer = descriptor,
            .set = materialInfo->descriptorSets[i].set,
            .descriptorIndex = index,
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


