// src/RenderEngine/RenderObjects/PipelineBuilder.cpp

#include "PipelineBuilder.hpp"

#include "RenderEngine/VkUtils.hpp"
#include <spdlog/spdlog.h>
#include <array>

PipelineBuilder::PipelineBuilder() {
    clear();
}

void PipelineBuilder::clear() {
    m_inputAssembly = {};
    m_inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

    m_rasterizer = {};
    m_rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

    m_colorBlendAttachment = {};

    m_multisampling = {};
    m_multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;

    m_depthStencil = {};
    m_depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

    m_renderInfo = {};
    m_renderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;

    m_vertexInputState = {};
    m_vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    m_colorAttachmentFormat = VK_FORMAT_UNDEFINED;

    m_shaderStages.clear();
    m_pushConstants.clear();
    m_descriptors.clear();
}

PipelineInfo PipelineBuilder::build(VkDevice device) {
    PipelineInfo output = {
        .pipeline = nullptr,
        .layout = nullptr,
        .success = true,
    };

    VkPipelineLayoutCreateInfo layoutInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .setLayoutCount = static_cast<U32>(m_descriptors.size()),
        .pSetLayouts = m_descriptors.data(),
        .pushConstantRangeCount = static_cast<U32>(m_pushConstants.size()),
        .pPushConstantRanges = m_pushConstants.data()
    };

    VkResult layoutResult = vkCreatePipelineLayout(
            device,
            &layoutInfo,
            nullptr,
            &output.layout
    );
    if (!VkUtils::checkVkResult(layoutResult, "Couldn't create pipeline layout")) {
        output.success = false;
        return output;
    }

    VkPipelineViewportStateCreateInfo viewportInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .viewportCount = 1,
        .pViewports = nullptr,
        .scissorCount = 1,
        .pScissors = nullptr,
    };

    VkPipelineColorBlendStateCreateInfo blendStateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_NO_OP,
        .attachmentCount = 1,
        .pAttachments = &m_colorBlendAttachment,
        .blendConstants = {},
    };

    std::array<VkDynamicState, 2> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamicStateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .dynamicStateCount = static_cast<U32>(dynamicStates.size()),
        .pDynamicStates = dynamicStates.data(),
    };

    VkGraphicsPipelineCreateInfo pipelineInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &m_renderInfo,
        .flags = 0,
        .stageCount = static_cast<U32>(m_shaderStages.size()),
        .pStages = m_shaderStages.data(),
        .pVertexInputState = &m_vertexInputState,
        .pInputAssemblyState = &m_inputAssembly,
        .pTessellationState = nullptr,
        .pViewportState = &viewportInfo,
        .pRasterizationState = &m_rasterizer,
        .pMultisampleState = &m_multisampling,
        .pDepthStencilState = &m_depthStencil,
        .pColorBlendState = &blendStateInfo,
        .pDynamicState = &dynamicStateInfo,
        .layout = output.layout,
        .renderPass = nullptr,
        .subpass = 0,
        .basePipelineHandle = nullptr,
        .basePipelineIndex = 0,
    };

    VkResult pipelineResult = vkCreateGraphicsPipelines(
            device,
            nullptr,
            1,
            &pipelineInfo,
            nullptr,
            &output.pipeline
    );
    if (!VkUtils::checkVkResult(pipelineResult, "Couldn't create pipeline")) {
        output.success = false;
        return output;
    }

    return output;
}

PipelineBuilder* PipelineBuilder::addShader(VkShaderModule module, VkShaderStageFlagBits stageFlags) {
    const char* name = "main";

    VkPipelineShaderStageCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = stageFlags,
        .module = module,
        .pName = name,
        .pSpecializationInfo = nullptr,
    };

    m_shaderStages.push_back(info);

    return this;
}

void PipelineBuilder::clearShaders() {
    m_shaderStages.clear();
}

PipelineBuilder* PipelineBuilder::setBlending(BlendingMode mode) {
    m_colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    switch (mode) {
        case BlendingMode::None:
            m_colorBlendAttachment.blendEnable = VK_FALSE;
            m_colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
            m_colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
            m_colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
            m_colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            m_colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            m_colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
            break;

        case BlendingMode::Additive:
            m_colorBlendAttachment.blendEnable = VK_TRUE;
            m_colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            m_colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
            m_colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
            m_colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            m_colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            m_colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
            break;

        case BlendingMode::Alpha:
            m_colorBlendAttachment.blendEnable = VK_TRUE;
            m_colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            m_colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            m_colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
            m_colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            m_colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            m_colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
            break;

        default:
            spdlog::error("Unsupported blending mode: {}", (int)mode);
            break;
    }

    return this;
}

PipelineBuilder* PipelineBuilder::setColorFormat(VkFormat format) {
    m_colorAttachmentFormat = format;

    m_renderInfo.colorAttachmentCount = 1;
    m_renderInfo.pColorAttachmentFormats = &m_colorAttachmentFormat;

    return this;
}

PipelineBuilder* PipelineBuilder::setDepthFormat(VkFormat format) {
    m_renderInfo.depthAttachmentFormat = format;

    return this;
}

PipelineBuilder* PipelineBuilder::setMultiSampling(VkSampleCountFlagBits samples) {
    m_multisampling.rasterizationSamples = samples;
    m_multisampling.sampleShadingEnable = VK_FALSE;
    m_multisampling.minSampleShading = 1.0f;
    m_multisampling.pSampleMask = nullptr;
    m_multisampling.alphaToCoverageEnable = VK_FALSE;
    m_multisampling.alphaToOneEnable = VK_FALSE;

    return this;
}

PipelineBuilder* PipelineBuilder::setPolygonMode(VkPolygonMode mode) {
    m_rasterizer.polygonMode = mode;
    m_rasterizer.lineWidth = 1.0f;

    return this;
}

PipelineBuilder* PipelineBuilder::setCullMode(VkCullModeFlags mode, VkFrontFace frontFace) {
    m_rasterizer.cullMode = mode;
    m_rasterizer.frontFace = frontFace;

    return this;
}

PipelineBuilder* PipelineBuilder::setInputTopology(VkPrimitiveTopology topology) {
    m_inputAssembly.topology = topology;
    m_inputAssembly.primitiveRestartEnable = VK_FALSE;

    return this;
}

PipelineBuilder* PipelineBuilder::setDepthInfo(bool depthTest, bool writeDepth, VkCompareOp op) {
    m_depthStencil.depthTestEnable = depthTest ? VK_TRUE : VK_FALSE;
    m_depthStencil.depthWriteEnable = writeDepth ? VK_TRUE : VK_FALSE;
    m_depthStencil.depthCompareOp = op;
    m_depthStencil.depthBoundsTestEnable = VK_FALSE;
    m_depthStencil.stencilTestEnable = VK_FALSE;

    // Init defaults
    m_depthStencil.front = {};
    m_depthStencil.back = {};

    return this;
}

PipelineBuilder* PipelineBuilder::addDescriptorLayout(VkDescriptorSetLayout layout) {
    m_descriptors.push_back(layout);

    return this;
};

PipelineBuilder* PipelineBuilder::addPushConstant(VkShaderStageFlags stageFlags, U32 size, U32 offset) {
    VkPushConstantRange range = {
        .stageFlags = stageFlags,
        .offset = offset,
        .size = size,
    };

    m_pushConstants.push_back(range);

    return this;
}

// I reference verticies via index in an array so this is unnessesary
PipelineBuilder* PipelineBuilder::setNoVertexInputState() {
    m_vertexInputState.vertexBindingDescriptionCount = 0;
    m_vertexInputState.pVertexBindingDescriptions = nullptr;
    m_vertexInputState.vertexAttributeDescriptionCount = 0;
    m_vertexInputState.pVertexAttributeDescriptions = nullptr;

    return this;
}

PipelineBuilder* PipelineBuilder::setVertexInputState(
    const std::vector<VkVertexInputBindingDescription>& bindingDescriptions,
    const std::vector<VkVertexInputAttributeDescription>& attributeDescriptions
) {
    m_vertexInputState.vertexBindingDescriptionCount = bindingDescriptions.size();
    m_vertexInputState.pVertexBindingDescriptions = bindingDescriptions.data();
    m_vertexInputState.vertexAttributeDescriptionCount = attributeDescriptions.size();
    m_vertexInputState.pVertexAttributeDescriptions = attributeDescriptions.data();

    return this;
}

