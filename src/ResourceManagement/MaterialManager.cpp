// src/ResourceManagement/MaterialManager.cpp

#include "MaterialManager.hpp"
#include "RenderEngine/Config.hpp"
#include "RenderEngine/RenderObjects/DescriptorLayoutBuilder.hpp"
#include "RenderEngine/RenderObjects/Materials.hpp"
#include "RenderEngine/RenderObjects/PipelineBuilder.hpp"
#include "ShaderLoading.hpp"
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

bool MaterialManager::initialize(std::shared_ptr<VulkanInfo> vkInfo) {
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
        {"VK_FORMAT_R8G8B8A8_UNORM", VK_FORMAT_R8G8B8A8_UNORM},
        {"VK_FORMAT_R16G16B16A16_SFLOAT", VK_FORMAT_R16G16B16A16_SFLOAT},
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

VkPolygonMode getPolygonMode(std::string input) {
    std::unordered_map<std::string, VkPolygonMode> map = {
        {"fill", VK_POLYGON_MODE_FILL},
        {"line", VK_POLYGON_MODE_LINE},
        {"point", VK_POLYGON_MODE_POINT},
        {"fill_rectangle_NV", VK_POLYGON_MODE_FILL_RECTANGLE_NV},
    };

    auto it = map.find(input);
    if (it != map.end()) {
        return it->second;
    } else {
        spdlog::warn("Polygon mode not recognized: {}", input);
        return VK_POLYGON_MODE_FILL;
    }
}

VkCullModeFlags getCullMode(std::string input) {
    std::unordered_map<std::string, VkCullModeFlags> map = {
        {"none", VK_CULL_MODE_NONE},
        {"front", VK_CULL_MODE_FRONT_BIT},
        {"back", VK_CULL_MODE_BACK_BIT},
        {"both", VK_CULL_MODE_FRONT_AND_BACK},
    };

    auto it = map.find(input);
    if (it != map.end()) {
        return it->second;
    } else {
        spdlog::warn("Cull mode not recognized: {}", input);
        return VK_CULL_MODE_NONE;
    }
}

VkFrontFace getFrontFace(std::string input) {
    std::unordered_map<std::string, VkFrontFace> map = {
        {"clockwise", VK_FRONT_FACE_CLOCKWISE},
        {"counter-clockwise", VK_FRONT_FACE_COUNTER_CLOCKWISE},
    };

    auto it = map.find(input);
    if (it != map.end()) {
        return it->second;
    } else {
        spdlog::warn("Front face not recognized: {}", input);
        return VK_FRONT_FACE_CLOCKWISE;
    }
}

VkPrimitiveTopology getTopology(std::string input) {
    std::unordered_map<std::string, VkPrimitiveTopology> map = {
        {"point-list", VK_PRIMITIVE_TOPOLOGY_POINT_LIST},
        {"line-list", VK_PRIMITIVE_TOPOLOGY_LINE_LIST},
        {"line-strip", VK_PRIMITIVE_TOPOLOGY_LINE_STRIP},
        {"triangle-list", VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST},
        {"triangle-strip", VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP},
        {"triangle-fan", VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN},
        {"line-list-with-adjacency", VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY},
        {"line-strip-with-adjacency", VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY},
        {"triangle-list-with-adjacency", VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY},
        {"triangle-strip-with-adjacency", VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY},
        {"patch-list", VK_PRIMITIVE_TOPOLOGY_PATCH_LIST}
    };

    auto it = map.find(input);
    if (it != map.end()) {
        return it->second;
    } else {
        spdlog::warn("Topology not recognized: {}", input);
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    }
}

VkCompareOp getCompareOp(std::string input) {
    std::unordered_map<std::string, VkCompareOp> map = {
        {"VK_COMPARE_OP_NEVER", VK_COMPARE_OP_NEVER},

        {"VK_COMPARE_OP_LESS", VK_COMPARE_OP_LESS},
        {"VK_COMPARE_OP_LESS_OR_EQUAL", VK_COMPARE_OP_LESS_OR_EQUAL},

        {"VK_COMPARE_OP_GREATER", VK_COMPARE_OP_GREATER},
        {"VK_COMPARE_OP_GREATER_OR_EQUAL", VK_COMPARE_OP_GREATER_OR_EQUAL},

        {"VK_COMPARE_OP_EQUAL", VK_COMPARE_OP_EQUAL},
        {"VK_COMPARE_OP_NOT_EQUAL", VK_COMPARE_OP_NOT_EQUAL},
    };

    auto it = map.find(input);
    if (it != map.end()) {
        return it->second;
    } else {
        spdlog::warn("Compare Op not recognized: {}", input);
        return VK_COMPARE_OP_NEVER;
    }
}

VkShaderStageFlagBits getShaderStageFlagBit(std::string stage) {
    std::unordered_map<std::string, VkShaderStageFlagBits> stageMap = {
        {"vertex", VK_SHADER_STAGE_VERTEX_BIT},
        {"fragment", VK_SHADER_STAGE_FRAGMENT_BIT},
        {"compute", VK_SHADER_STAGE_COMPUTE_BIT},
        {"geometry", VK_SHADER_STAGE_GEOMETRY_BIT},
        {"tessellation_control", VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT},
        {"tessellation_evaluation", VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT},
        {"all_graphics", VK_SHADER_STAGE_ALL_GRAPHICS},
        {"all", VK_SHADER_STAGE_ALL},
    };

    auto it = stageMap.find(stage);
    if (it != stageMap.end()) {
        return it->second;
    } else {
        spdlog::error("Shader stage not recognized: {}", stage);
        return VK_SHADER_STAGE_ALL;
    }
}


VkShaderStageFlags getShaderStageFlags(const std::vector<std::string>& stages) {
    std::unordered_map<std::string, VkShaderStageFlagBits> stageMap = {
        {"vertex", VK_SHADER_STAGE_VERTEX_BIT},
        {"fragment", VK_SHADER_STAGE_FRAGMENT_BIT},
        {"compute", VK_SHADER_STAGE_COMPUTE_BIT},
        {"geometry", VK_SHADER_STAGE_GEOMETRY_BIT},
        {"tessellation_control", VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT},
        {"tessellation_evaluation", VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT},
        {"all_graphics", VK_SHADER_STAGE_ALL_GRAPHICS},
        {"all", VK_SHADER_STAGE_ALL},
    };

    VkShaderStageFlags flags = 0;
    for (const auto& stage : stages) {
        auto it = stageMap.find(stage);
        if (it != stageMap.end()) {
            flags |= it->second;
        } else {
            spdlog::warn("Shader stage not recognized: {}", stage);
        }
    }

    return flags;
}

VkDescriptorType getDescriptorType(std::string input) {
    std::unordered_map<std::string, VkDescriptorType> map = {
        {"VK_DESCRIPTOR_TYPE_SAMPLER", VK_DESCRIPTOR_TYPE_SAMPLER},
        {"VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER", VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER},
        {"VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE", VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE},
        {"VK_DESCRIPTOR_TYPE_STORAGE_IMAGE", VK_DESCRIPTOR_TYPE_STORAGE_IMAGE},
        {"VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER", VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER},
        {"VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER", VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER},
        {"VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER", VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER},
        {"VK_DESCRIPTOR_TYPE_STORAGE_BUFFER", VK_DESCRIPTOR_TYPE_STORAGE_BUFFER},
        {"VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC", VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC},
        {"VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC", VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC},
        {"VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT", VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT},
        {"VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT", VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT},
        {"VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR", VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR},
        {"VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV", VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV}
    };

    auto it = map.find(input);
    if (it != map.end()) {
        return it->second;
    } else {
        spdlog::warn("Descriptor Type not recognized: {}", input);
        return it->second;
    }
}

std::vector<VkPushConstantRange> parsePushConstants(YAML::Node& yaml) {
    std::vector<VkPushConstantRange> pushConstants;

    if (!yaml["push_constants"]) {
        return pushConstants;
    }

    for (const auto& node : yaml["push_constants"]) {
        VkPushConstantRange range{};
        range.size = node["size"].as<uint32_t>();
        range.offset = node["offset"].as<uint32_t>();

        // Handle multiple shader stages
        if (node["stages"].IsSequence()) {
            std::vector<std::string> stages;
            for (const auto& stage : node["stages"]) {
                stages.push_back(stage.as<std::string>());
            }
            range.stageFlags = getShaderStageFlags(stages);
        } else {
            range.stageFlags = getShaderStageFlags({node["stages"].as<std::string>()});
        }

        pushConstants.push_back(range);
    }

    return pushConstants;
}

VkDescriptorSetLayout yamlToLayout(YAML::Node& yaml, VkDevice device) {
    DescriptorLayoutBuilder builder;

    YAML::Node bindings = yaml["descriptor_layout"]["bindings"];

    for (const YAML::Node& node : bindings) {
        uint32_t binding = node["binding"].as<uint32_t>();
        VkDescriptorType descriptorType = getDescriptorType(node["descriptor_type"].as<std::string>());

        // Convert shader stages
        VkShaderStageFlags stageFlags = 0;
        if (node["stages"].IsSequence()) {
            std::vector<std::string> stages;
            for (const auto& stage : node["stages"]) {
                stages.push_back(stage.as<std::string>());
            }
            stageFlags = getShaderStageFlags(stages);
        } else {
            stageFlags = getShaderStageFlags({node["stages"].as<std::string>()});
        }

        // Add binding to builder
        builder.addBinding(binding, descriptorType, stageFlags);
    }

    return builder.build(device);
}

VkDescriptorSetLayout MaterialManager::getLayout(std::string path) {
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
    VkDescriptorSetLayout layout = yamlToLayout(yaml, m_vkInfo->device);

    m_descriptorLayouts[path] = RefCount<VkDescriptorSetLayout>{
        .value = layout,
        .references = 1,
    };

    return m_descriptorLayouts[path].value;
}

MaterialInfo yamlToInfo(MaterialManager* materialManager, YAML::Node& yaml, fs::path& basePath, std::string& folder, VkDevice device) {
    PipelineBuilder builder;

    YAML::Node pipeline = yaml["pipeline"];
    YAML::Node depthInfo = pipeline["depth_info"];

    // Descriptors
    YAML::Node descriptors = pipeline["descriptor_layouts"];
    std::vector<VkDescriptorSetLayout> layouts;
    for (const YAML::Node& set : descriptors) {
        VkDescriptorSetLayout setLayout = materialManager->getLayout(
                fmt::format("{}/{}", folder, set.as<std::string>())
        );
        builder.addDescriptorLayout(setLayout);
        layouts.push_back(setLayout);
    }

    // Shaders
    YAML::Node shaders = pipeline["shaders"];
    std::vector<VkShaderModule> shaderModules;
    for (const YAML::Node& shader : shaders) {
        fs::path shaderPath = basePath / folder / shader["module"].as<std::string>();

        std::string shaderStage = shader["stage"].as<std::string>();
        VkShaderStageFlagBits stage = getShaderStageFlagBit(shaderStage);

        VkShaderModule shaderModule = LoadAndCompileShader(device, shaderPath, stage);

        builder.addShader(shaderModule, stage);
        shaderModules.push_back(shaderModule);
    }

    // Pipeline
    builder.setBlending(getBlendingMode(pipeline["blending"].as<std::string>()));
    builder.setColorFormat(getFormat(pipeline["color_format"].as<std::string>()));
    builder.setDepthFormat(Config::depthFormat);
    builder.setMultiSampling(getMultisampleCount(pipeline["multisampling"].as<std::string>()));
    builder.setPolygonMode(getPolygonMode(pipeline["polygon_mode"].as<std::string>()));
    builder.setCullMode(
            getCullMode(pipeline["cull_mode"].as<std::string>()),
            getFrontFace(pipeline["front_face"].as<std::string>())
    );
    builder.setInputTopology(getTopology(pipeline["topology"].as<std::string>()));
    builder.setDepthInfo(
            depthInfo["depth_test"].as<bool>(),
            depthInfo["write_depth"].as<bool>(),
            getCompareOp(depthInfo["compare_op"].as<std::string>())
    );

    std::vector<VkPushConstantRange> pushConstants = parsePushConstants(yaml);
    for (Size i = 0; i < pushConstants.size(); i++) {
        builder.addPushConstant(
                pushConstants[i].stageFlags,
                pushConstants[i].size,
                pushConstants[i].offset
        );
    }

    PipelineInfo piplineInfo = builder.build(device);
    MaterialInfo output = {
        .pipeline = piplineInfo.pipeline,
        .pipelineLayout = piplineInfo.layout,
        .descriptorLayouts = layouts,
        .type = MaterialType::Opaque,   // TODO: materialtypes
    };

    for (VkShaderModule module : shaderModules) {
        vkDestroyShaderModule(device, module, nullptr);
    }

    return output;
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
    MaterialInfo matInfo = yamlToInfo(this, yaml, resourceBasePath, path, m_vkInfo->device);

    m_materialInfos[path] = RefCount<MaterialInfo>{
        .value = matInfo,
        .references = 1,
    };

    return &m_materialInfos[path].value;
}

void MaterialManager::destroyMaterialInfo(MaterialInfo* info) {
    vkDestroyPipeline(m_vkInfo->device, info->pipeline, nullptr);
    vkDestroyPipelineLayout(m_vkInfo->device, info->pipelineLayout, nullptr);

    for (VkDescriptorSetLayout layout : info->descriptorLayouts) {
        this->dropLayout(layout);
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

    spdlog::error("Image not found for dropping!");
}

void MaterialManager::dropLayout(VkDescriptorSetLayout layout) {
    for (auto it = m_descriptorLayouts.begin(); it != m_descriptorLayouts.end(); ++it) {
        auto sharedResource = it->second.value;
        if (sharedResource == layout) {
            // Decrement reference count
            it->second.references--;

            if (it->second.references <= 0) {
                // If reference count is zero, apply custom shutdown logic
                vkDestroyDescriptorSetLayout(m_vkInfo->device, layout, nullptr);
                m_descriptorLayouts.erase(it);
            }
            return;
        }
    }

    spdlog::error("Image not found for dropping!");
}

MaterialData MaterialManager::getData(std::string path) {
    MaterialData data = {

    };

    return data;
}

void MaterialManager::dropMaterialData(MaterialData* data) {

}


