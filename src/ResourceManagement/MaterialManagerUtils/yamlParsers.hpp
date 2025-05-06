// src/ResourceManagement/MaterialManagerUtils/yamlParsers.hpp

#pragma once

#include "RenderEngine/RenderObjects/Materials.hpp"
#include "ResourceManagement/MaterialManager.hpp"
#include "ResourceManagement/RenderResources/VertexAttribute.hpp"

#include <yaml-cpp/yaml.h>

namespace MaterialManagerUtils {

Result<DescriptorSetInfo, std::string> yamlToLayout(
    YAML::Node& yaml,
    VkDevice device
);

Result<MaterialInfo, std::string> yamlToInfo(
    MaterialManager* materialManager,
    YAML::Node& yaml,
    fs::path& basePath,
    std::string& folder,
    VkDevice device,
    const ProvidedVertexLayout* providedLayout
);

}
