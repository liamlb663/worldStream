// src/ResourceManagement/RenderResources/RenderObjects/PipelineBuilder.hpp

#pragma once

#include "Core/Types.hpp"
#include "RenderEngine/VulkanInfo.hpp"
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

class PipelineBuilder {
public:

    void build() {
        VkPipelineLayoutCreateInfo layoutInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = static_cast<U32>(descriptors.size()),
            .pSetLayouts = descriptors.data(),
            .pushConstantRangeCount = static_cast<U32>(pushConstants.size()),
            .pPushConstantRanges = pushConstants.data()
        };

        vkCreatePipelineLayout(
                m_vkInfo->device,
                &layoutInfo,
                nullptr,
                &pipelineLayout
        );

        VkGraphicsPipelineCreateInfo pipelineInfo = {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
        };

        vkCreateGraphicsPipelines(
                m_vkInfo->device,
                nullptr,
                1,
                &pipelineInfo,
                nullptr,
                &pipeline
        );
    }

    std::shared_ptr<VulkanInfo> m_vkInfo;
    std::vector<VkDescriptorSetLayout> descriptors;
    std::vector<VkPushConstantRange> pushConstants;

    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;

};
