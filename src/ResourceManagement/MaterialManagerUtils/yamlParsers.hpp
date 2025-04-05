// src/ResourceManagement/MaterialManagerUtils/yamlParsers.hpp

#pragma once

#include "RenderEngine/RenderObjects/Materials.hpp"
#include "ResourceManagement/MaterialManager.hpp"

#include <yaml-cpp/yaml.h>

namespace MaterialManagerUtils {

Result<DescriptorLayoutInfo, std::string> yamlToLayout(
    YAML::Node& yaml,
    VkDevice device
);

Result<MaterialInfo, std::string> yamlToInfo(
    MaterialManager* materialManager,
    YAML::Node& yaml,
    fs::path& basePath,
    std::string& folder,
    VkDevice device
);

}
