// src/ResourceManagement/MaterialManagerUtils/yamlParsers.cpp

#include "yamlParsers.hpp"
#include "spdlog/spdlog.h"
#include "stringParsers.hpp"

#include "RenderEngine/Config.hpp"
#include "RenderEngine/RenderObjects/PipelineBuilder.hpp"
#include "ResourceManagement/MaterialManager.hpp"
#include "RenderEngine/RenderObjects/DescriptorSetBuilder.hpp"
#include "RenderEngine/RenderObjects/Materials.hpp"
#include "ShaderLoading.hpp"

#include "Core/Types.hpp"
#include <vulkan/vulkan.h>
#include <fmt/format.h>

#include <string>

namespace MaterialManagerUtils {

Result<DescriptorSetInfo, std::string> yamlToLayout(YAML::Node& yaml, VkDevice device) {
    DescriptorSetBuilder builder;

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

        U32 size = node["size"].as<U32>();
        U32 offset = node["offset"].as<U32>();

        // Add binding to builder
        builder.addBinding(binding, descriptorType, stageFlags, size, offset);
    }

    return builder.build(device).value();
}

// Info Stuff

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

std::pair<
    std::vector<VkVertexInputBindingDescription>,
    std::vector<VkVertexInputAttributeDescription>
> parseVertexInput(YAML::Node& yaml) {
    std::vector<VkVertexInputBindingDescription> bindings;
    std::vector<VkVertexInputAttributeDescription> attributes;

    if (!yaml["vertex_input"]) {
        return {bindings, attributes};
    }

    YAML::Node input = yaml["vertex_input"];

    if (input["bindings"]) {
        for (const auto& node : input["bindings"]) {
            VkVertexInputBindingDescription desc{};
            desc.binding = node["binding"].as<uint32_t>();
            desc.stride = node["stride"].as<uint32_t>();

            std::string rate = node["input_rate"].as<std::string>();
            if (rate == "vertex") {
                desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            } else if (rate == "instance") {
                desc.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
            } else {
                throw std::runtime_error("Unknown input_rate: " + rate);
            }

            bindings.push_back(desc);
        }
    }

    if (input["attributes"]) {
        for (const auto& node : input["attributes"]) {
            VkVertexInputAttributeDescription desc{
                .location = node["location"].as<uint32_t>(),
                .binding = node["binding"].as<uint32_t>(),
                .format = getFormat(node["format"].as<std::string>()),
                .offset = node["offset"].as<uint32_t>(),
            };

            attributes.push_back(desc);
        }
    }

    return {bindings, attributes};
}

Result<MaterialInfo, std::string> yamlToInfo(MaterialManager* materialManager, YAML::Node& yaml, fs::path& basePath, std::string& folder, VkDevice device) {
    PipelineBuilder builder;

    YAML::Node pipeline = yaml["pipeline"];
    YAML::Node depthInfo = pipeline["depth_info"];

    // Descriptors
    YAML::Node descriptors = pipeline["descriptor_layouts"];
    std::vector<DescriptorSetInfo> layouts;
    for (const YAML::Node& set : descriptors) {
        DescriptorSetInfo setLayout = materialManager->getLayout(
                fmt::format("{}/{}", folder, set["layout"].as<std::string>())
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

    std::vector<VkPushConstantRange> pushConstants = parsePushConstants(pipeline);
    PushConstantsInfo pushConstantsInfo = {};
    pushConstantsInfo.enabled = false;

    for (Size i = 0; i < pushConstants.size(); i++) {
        builder.addPushConstant(
                pushConstants[i].stageFlags,
                pushConstants[i].size,
                pushConstants[i].offset
        );

        pushConstantsInfo = {
            .enabled = true,
            .stages = pushConstants[i].stageFlags,
            .size = pushConstants[i].size,
            .offset = pushConstants[i].offset,
        };
    }

    auto [bindings, attributes] = parseVertexInput(pipeline);
    builder.setVertexInputState(bindings, attributes);

    PipelineInfo piplineInfo = builder.build(device);
    MaterialInfo output = {
        .pipeline = piplineInfo.pipeline,
        .pipelineLayout = piplineInfo.layout,
        .pushConstants = pushConstantsInfo,
        .descriptorSets = layouts,
        .type = MaterialType::Opaque,   // TODO: materialtypes
    };

    for (VkShaderModule module : shaderModules) {
        vkDestroyShaderModule(device, module, nullptr);
    }

    return output;
}

}
