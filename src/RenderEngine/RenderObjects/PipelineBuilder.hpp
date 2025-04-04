// src/RenderEngine/RenderObjects/PipelineBuilder.hpp

#pragma once

#include "Core/Types.hpp"
#include "RenderEngine/RenderObjects/Materials.hpp"
#include <vector>
#include <vulkan/vulkan.h>

enum class BlendingMode {
    None,
    Additive,
    Alpha,
};

struct PipelineInfo {
    VkPipeline pipeline;
    VkPipelineLayout layout;
    bool success;
};

class PipelineBuilder {
public:
    PipelineBuilder();
    void clear();
    PipelineInfo build(VkDevice device);

    PipelineBuilder* addShader(VkShaderModule module, VkShaderStageFlagBits stageFlags);
    void clearShaders();

    PipelineBuilder* setBlending(BlendingMode mode);

    PipelineBuilder* setColorFormat(VkFormat format);
    PipelineBuilder* setDepthFormat(VkFormat format);

    PipelineBuilder* setMultiSampling(VkSampleCountFlagBits samples);

    PipelineBuilder* setPolygonMode(VkPolygonMode mode);
    PipelineBuilder* setCullMode(VkCullModeFlags mode, VkFrontFace frontFace);

    PipelineBuilder* setInputTopology(VkPrimitiveTopology topology);

    PipelineBuilder* setDepthInfo(bool depthTest, bool writeDepth, VkCompareOp op);

    PipelineBuilder* addDescriptorLayout(DescriptorLayoutInfo layout);
    PipelineBuilder* addPushConstant(VkShaderStageFlags stageFlags, U32 size, U32 offset);

    PipelineBuilder* setNoVertexInputState();
    PipelineBuilder* setVertexInputState(
        const std::vector<VkVertexInputBindingDescription>& bindingDescriptions,
        const std::vector<VkVertexInputAttributeDescription>& attributeDescriptions
    );

private:
    std::vector<VkDescriptorSetLayout> m_descriptors;
    std::vector<VkPushConstantRange> m_pushConstants;
    std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;

    std::vector<VkVertexInputBindingDescription> m_vertexBindings;
    std::vector<VkVertexInputAttributeDescription> m_vertexAttributes;

    VkPipelineInputAssemblyStateCreateInfo m_inputAssembly;
    VkPipelineRasterizationStateCreateInfo m_rasterizer;
    VkPipelineColorBlendAttachmentState m_colorBlendAttachment;
    VkPipelineMultisampleStateCreateInfo m_multisampling;
    VkPipelineDepthStencilStateCreateInfo m_depthStencil;
    VkPipelineRenderingCreateInfo m_renderInfo;
    VkPipelineVertexInputStateCreateInfo m_vertexInputState;

    VkFormat m_colorAttachmentFormat;
};
