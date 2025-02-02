// src/Game/RenderGraphSetup.cpp

#include "RenderGraphSetup.hpp"

#include "RenderEngine/Config.hpp"
#include "RenderEngine/FrameSubmitInfo.hpp"
#include <RenderEngine/CommandSubmitter.hpp>

#include <memory>

std::shared_ptr<RenderGraph> setupRenderGraph() {
    auto renderGraph = std::make_shared<RenderGraph>();

    VkImageUsageFlags commonFlags = 0;
    commonFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    commonFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    commonFlags |= VK_IMAGE_USAGE_STORAGE_BIT;
    commonFlags |= VK_IMAGE_USAGE_SAMPLED_BIT;

    Size finalImg = renderGraph->addImage(
        Vector<U32, 2>(0),
        Vector<F32, 2>(1),
        ImageSizeType::fractional,
        Config::drawFormat,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | commonFlags,
        "Final Draw"
    );

    Size geometry = renderGraph->addGeometry("Main Geometry");
    Size geometryPass = renderGraph->createNode(
        "Geometry",
        [finalImg, geometry](RecordInfo recordInfo) {
            std::shared_ptr<Image> outputImg = recordInfo.renderContext->images[finalImg];

            recordInfo.commandSubmitter->transitionImage(
                recordInfo.commandBuffer,
                outputImg,
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
            );

            VkRenderingAttachmentInfo colorAttachment = {
                .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
                .pNext = nullptr,
                .imageView = outputImg->view,
                .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
                .resolveMode = VK_RESOLVE_MODE_NONE,
                .resolveImageView = nullptr,
                .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .clearValue = {.color = {{1.0f, 1.0f, 1.0f, 1.0f}}},
            };

            VkRenderingInfo renderingInfo = {
                .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
                .pNext = nullptr,
                .flags = 0,
                .renderArea = {.offset = {0, 0}, .extent = outputImg->size},
                .layerCount = 1,
                .viewMask = 0,
                .colorAttachmentCount = 1,
                .pColorAttachments = &colorAttachment,
                .pDepthAttachment = nullptr,
                .pStencilAttachment = nullptr,
            };

            vkCmdBeginRendering(recordInfo.commandBuffer, &renderingInfo);

            VkViewport viewport = {
                .x = 0.0f, .y = 0.0f,
                .width = static_cast<float>(outputImg->size.value.x),
                .height = static_cast<float>(outputImg->size.value.y),
                .minDepth = 0.0f, .maxDepth = 1.0f
            };
            vkCmdSetViewport(recordInfo.commandBuffer, 0, 1, &viewport);

            VkRect2D scissor = {.offset = {0, 0}, .extent = outputImg->size};
            vkCmdSetScissor(recordInfo.commandBuffer, 0, 1, &scissor);

            MaterialInfo* material = recordInfo.renderContext->geometries[geometry][0].material->pipeline;

            vkCmdBindPipeline(recordInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipeline);
            vkCmdDraw(recordInfo.commandBuffer, 3, 1, 0, 0);

            vkCmdEndRendering(recordInfo.commandBuffer);
        },
        {}
    );

    renderGraph->addGeometryInput(geometryPass, {geometry});
    renderGraph->addImageOutput(geometryPass, {finalImg});

    Size postFxPass = renderGraph->createNode(
        "Post Fx",
        [finalImg](RecordInfo recordInfo) {
            std::shared_ptr<Image> outputImg = recordInfo.renderContext->images[finalImg];

            recordInfo.commandSubmitter->transitionImage(
                recordInfo.commandBuffer,
                outputImg,
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
            );

            recordInfo.commandSubmitter->transitionVulkanImage(
                recordInfo.commandBuffer,
                recordInfo.swapchainImage->image,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
            );

            VkOffset3D blitSize = {
                .x = static_cast<I32>(outputImg->size.value.x),
                .y = static_cast<I32>(outputImg->size.value.y),
                .z = 1,
            };

            VkImageSubresourceLayers subLayers = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = 1,
            };

            VkImageBlit blitInfo = {
                .srcSubresource = subLayers,
                .srcOffsets = {{}, blitSize},
                .dstSubresource = subLayers,
                .dstOffsets = {{}, blitSize}
            };

            vkCmdBlitImage(
                recordInfo.commandBuffer,
                outputImg->image,
                outputImg->layout,
                recordInfo.swapchainImage->image,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1, &blitInfo,
                VkFilter::VK_FILTER_NEAREST
            );

            recordInfo.commandSubmitter->transitionVulkanImage(
                recordInfo.commandBuffer,
                recordInfo.swapchainImage->image,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
            );
        },
        {geometryPass}
    );

    renderGraph->addImageInput(postFxPass, {finalImg});
    renderGraph->addImageOutput(postFxPass, {});

    return renderGraph;
}

