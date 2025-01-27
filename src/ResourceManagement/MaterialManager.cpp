// src/ResourceManagement/MaterialManager.cpp

#include "MaterialManager.hpp"
#include "RenderEngine/Config.hpp"
#include "RenderEngine/RenderObjects/Materials.hpp"
#include "RenderEngine/RenderObjects/PipelineBuilder.hpp"
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

bool MaterialManager::initialize(std::shared_ptr<VulkanInfo> vkInfo) {
    m_vkInfo = vkInfo;
    return true;
}

void MaterialManager::shutdown() {

}

BlendingMode getBlendingMode(std::string input) {
    std::unordered_map<std::string, BlendingMode> map = {
        {"None", BlendingMode::None},
        {"Alpha", BlendingMode::Alpha},
        {"Additive", BlendingMode::Additive},
    };

    auto it = map.find(input);
    if (it != map.end()) {
        return it->second;
    } else {
        spdlog::warn("BlendingMode not recognized: {}", input);
        return BlendingMode::None;
    }
}

VkFormat getFormat(std::string input) {
    std::unordered_map<std::string, VkFormat> map = {
        {"R8G8B8A8_UNORM", VK_FORMAT_R8G8B8A8_UNORM},
        {"B8G8R8A8_UNORM", VK_FORMAT_B8G8R8A8_UNORM},
        {"R8_UNORM", VK_FORMAT_R8_UNORM},
        {"R16G16B16A16_SFLOAT", VK_FORMAT_R16G16B16A16_SFLOAT},
        {"D32_SFLOAT", VK_FORMAT_D32_SFLOAT},
        {"D32_SFLOAT_S8_UINT", VK_FORMAT_D32_SFLOAT_S8_UINT},
        {"D24_UNORM_S8_UINT", VK_FORMAT_D24_UNORM_S8_UINT},
        {"R8G8B8_UNORM", VK_FORMAT_R8G8B8_UNORM},
    };

    auto it = map.find(input);
    if (it != map.end()) {
        return it->second;
    } else {
        spdlog::warn("VkFormat not recognized: {}", input);
        return VK_FORMAT_UNDEFINED;
    }
}

VkSampleCountFlagBits getMultisampleCount(std::string input) {
    std::unordered_map<std::string, VkSampleCountFlagBits> map = {
        {"VK_SAMPLE_COUNT_1_BIT", VK_SAMPLE_COUNT_1_BIT},
        {"VK_SAMPLE_COUNT_2_BIT", VK_SAMPLE_COUNT_2_BIT},
        {"VK_SAMPLE_COUNT_4_BIT", VK_SAMPLE_COUNT_4_BIT},
        {"VK_SAMPLE_COUNT_8_BIT", VK_SAMPLE_COUNT_8_BIT},
        {"VK_SAMPLE_COUNT_16_BIT", VK_SAMPLE_COUNT_16_BIT},
        {"VK_SAMPLE_COUNT_32_BIT", VK_SAMPLE_COUNT_32_BIT},
        {"VK_SAMPLE_COUNT_64_BIT", VK_SAMPLE_COUNT_64_BIT},
    };

    auto it = map.find(input);
    if (it != map.end()) {
        return it->second;
    } else {
        spdlog::warn("Sample count not recognized: {}", input);
        return VK_SAMPLE_COUNT_1_BIT;
    }
}

MaterialInfo yamlToInfo(YAML::Node& yaml, VkDevice device) {
    PipelineBuilder builder;

    YAML::Node pipeline = yaml["pipeline"];

    builder.setBlending(getBlendingMode(pipeline["blending"].as<std::string>()));
    builder.setColorFormat(getFormat(pipeline["color_format"].as<std::string>()));
    builder.setDepthFormat(Config::depthFormat);
    builder.setMultiSampling(getMultisampleCount(pipeline["multisampling"].as<std::string>()));

    PipelineInfo piplineInfo = builder.build(device);
    MaterialInfo output = {
        .pipeline = piplineInfo.pipeline,
        .pipelineLayout = piplineInfo.layout,
        .descriptorLayouts = {},
        .type = MaterialType::Opaque,
    };

    return output;
}

MaterialInfo* MaterialManager::getInfo(std::string path) {
    auto it = m_materialInfos.find(path);
    if (it != m_materialInfos.end()) {
        it->second.references++;
        return &it->second.value;
    }

    fs::path fullPath = resourceBasePath / path;

    if (!fs::exists(fullPath)) {
        spdlog::error("Material Descriptor not found: {}", fullPath.string());
    }

    // Get Material Info
    YAML::Node yaml = YAML::LoadFile(fullPath);
    MaterialInfo matInfo = yamlToInfo(yaml, m_vkInfo->device);

    m_materialInfos[path] = RefCount<MaterialInfo>{
        .value = matInfo,
        .references = 1,
    };

    return &m_materialInfos[path].value;
}
