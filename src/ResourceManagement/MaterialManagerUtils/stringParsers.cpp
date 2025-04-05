// src/ResourceManagement/MaterialManagerUtils/stringParsers.cpp

#include "stringParsers.hpp"

namespace MaterialManagerUtils {

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
        {"VK_FORMAT_R32_SFLOAT", VK_FORMAT_R32_SFLOAT},
        {"VK_FORMAT_R32G32_SFLOAT", VK_FORMAT_R32G32_SFLOAT},
        {"VK_FORMAT_R32G32B32_SFLOAT", VK_FORMAT_R32G32B32_SFLOAT},
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

}

