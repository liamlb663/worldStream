// src/RenderEngine/CommandSubmitter.cpp

#include "CommandSubmitter.hpp"

#include "InternalResources/CommandPool.hpp"
#include "VkUtils.hpp"

#include <spdlog/spdlog.h>
#include <vulkan/vk_enum_string_helper.h>

bool CommandSubmitter::initialize(std::shared_ptr<VulkanInfo> vkInfo) {
    m_vkInfo = vkInfo;

    return true;
}

void CommandSubmitter::transferSubmit(const std::function<void(VkCommandBuffer)>& function) {
    // Get Buffer
    VkCommandBuffer commandBuffer = m_vkInfo->transferPool->getBuffer(0);

    // Begin Command Buffer
    VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr,
    };

    VkResult res;
    res = vkBeginCommandBuffer(commandBuffer, &beginInfo);
    if (!VkUtils::checkVkResult(res, "Failed to begin recording transfer command buffer.")) {
        return;
    }

    // Execute the lambda to record commands
    function(commandBuffer);

    // End recording
    res = vkEndCommandBuffer(commandBuffer);
    if (!VkUtils::checkVkResult(res, "Failed to record transfer command buffer.")) {
        return;
    }

    VkSubmitInfo submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,

        .waitSemaphoreCount = 0,
        .pWaitSemaphores = nullptr,

        .pWaitDstStageMask = nullptr,

        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer,

        .signalSemaphoreCount = 0,
        .pSignalSemaphores = nullptr,
    };

    res = vkQueueSubmit(m_vkInfo->transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
    if (!VkUtils::checkVkResult(res, "Failed to submit transfer command buffer.")) {
        return;
    }

    // Block on transfer completion
    vkQueueWaitIdle(m_vkInfo->transferQueue);
}

void CommandSubmitter::frameSubmit(FrameSubmitInfo info) {
    FrameData frame = info.frameData;
    std::shared_ptr<RenderGraph> graph = frame.renderGraph;

    info.frameData.commandPool.resetPool();

    Size numNodes = graph->nodes.size();

    std::vector<VkCommandBuffer>& cmdBuffers = info.frameData.commandPool.getBuffers();

    VkPipelineStageFlags pipelineStageFlags = 0;
    //pipelineStageFlags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    //pipelineStageFlags |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
    //pipelineStageFlags |= VK_PIPELINE_STAGE_TRANSFER_BIT;
    pipelineStageFlags |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    for (Size i = 0; i < numNodes; i++) {
        VkCommandBuffer cmd = cmdBuffers[i];

        VkCommandBufferBeginInfo beginInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = nullptr,
        };

        VkResult res = vkBeginCommandBuffer(cmd, &beginInfo);
        if (!VkUtils::checkVkResult(res, "Failed to begin recording transfer command buffer.")) {
            return;
        }

        // Execute the lambda to record commands
        RecordInfo recordInfo = {
            .commandBuffer = cmd,
            .renderContext = &info.frameData.renderContext,
            .swapchainImage = &info.swapchainImage,
            .commandSubmitter = this,
        };
        graph->nodes[i].execute(recordInfo);

        // End recording
        res = vkEndCommandBuffer(cmd);
        if (!VkUtils::checkVkResult(res, "Failed to record transfer command buffer.")) {
            return;
        }

        // Resolve dependencies
        std::vector<Size>& depIndecies = frame.renderGraph->adjacency[i];
        std::vector<VkSemaphore> dependencys;
        for (Size j : depIndecies) {
            VkSemaphore depSemaphore = frame.renderContext.semaphores[j].get();
            if (!depSemaphore) {
                spdlog::error("Dependency semaphore at index {} is null!", j);
                return;
            }
            dependencys.push_back(depSemaphore);
        }

        VkSemaphore signalSemaphore = frame.renderContext.semaphores[i].get();
        if (!signalSemaphore) {
            spdlog::error("Signal semaphore at index {} is null!", i);
            return;
        }

        VkFence renderFence = nullptr;
        if (i == numNodes - 1) {
            dependencys.push_back(frame.swapchainSemaphore.get());
            renderFence = info.frameData.renderFence.get();
        }

        std::vector<VkPipelineStageFlags> waitStages(dependencys.size(), pipelineStageFlags);

        VkSubmitInfo submitInfo{
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = static_cast<U32>(dependencys.size()),
            .pWaitSemaphores = dependencys.data(),
            .pWaitDstStageMask = waitStages.data(),
            .commandBufferCount = 1,
            .pCommandBuffers = &cmdBuffers[i],
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &signalSemaphore,
        };

        res = vkQueueSubmit(m_vkInfo->graphicsQueue, 1, &submitInfo, renderFence);
        if (!VkUtils::checkVkResult(res, "Failed to submit transfer command buffer.")) {
            return;
        }
    }
}

// Holy Shit
struct StageAccessMasks {
    VkPipelineStageFlags2 stageMask;
    VkAccessFlags2 accessMask;
};

StageAccessMasks getStageAccessMasks(VkImageLayout layout) {
    StageAccessMasks masks = {};

    switch (layout) {
        case VK_IMAGE_LAYOUT_UNDEFINED:
            masks.stageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
            masks.accessMask = VK_ACCESS_2_NONE; // No previous access
            break;

        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            masks.stageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
            masks.accessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            masks.stageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT; // Or COMPUTE/GRAPHICS as needed
            masks.accessMask = VK_ACCESS_2_SHADER_READ_BIT;
            break;

        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            masks.stageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
            masks.accessMask = VK_ACCESS_2_TRANSFER_READ_BIT;
            break;

        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            masks.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
            masks.accessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
            masks.stageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
            masks.accessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            masks.stageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
            masks.accessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
            masks.stageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
            masks.accessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
            break;

        case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
            masks.stageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
            //masks.accessMask = VK_ACCESS_2_MEMORY_READ_BIT;
            masks.accessMask = 0;
            break;

        case VK_IMAGE_LAYOUT_GENERAL:
            masks.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
            masks.accessMask = VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT;
            break;

        default:
            // Handle unsupported layouts or add more cases as needed
            masks.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
            masks.accessMask = VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT;
            spdlog::warn("Using unsupported Image Layout");
            break;
    }

    return masks;
}

void CommandSubmitter::transitionVulkanImage(VkCommandBuffer cmd, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout) {
    if (newLayout == VK_IMAGE_LAYOUT_UNDEFINED) {
        spdlog::error("Invalid layout transition: newLayout cannot be VK_IMAGE_LAYOUT_UNDEFINED.");
        return;
    }

    if (oldLayout == newLayout) {
        spdlog::warn("Skipping transition: Image is already in the desired layout.");
        return;
    }

    StageAccessMasks srcMasks = getStageAccessMasks(oldLayout);
    StageAccessMasks dstMasks = getStageAccessMasks(newLayout);

    VkImageAspectFlags aspectMask =
            (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL)
            ? VK_IMAGE_ASPECT_DEPTH_BIT
            : VK_IMAGE_ASPECT_COLOR_BIT;

    VkImageSubresourceRange subImage = {};
    subImage.aspectMask = aspectMask;
    subImage.baseMipLevel = 0;
    subImage.baseArrayLayer = 0;
    subImage.levelCount = VK_REMAINING_MIP_LEVELS;
    subImage.layerCount = VK_REMAINING_ARRAY_LAYERS;

    VkImageMemoryBarrier2 imageBarrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .pNext = nullptr,
        .srcStageMask   = srcMasks.stageMask,
        .srcAccessMask  = srcMasks.accessMask,
        .dstStageMask   = dstMasks.stageMask,
        .dstAccessMask  = dstMasks.accessMask,
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .srcQueueFamilyIndex = 0,
        .dstQueueFamilyIndex = 0,
        .image = image,
        .subresourceRange = subImage,
    };

    VkDependencyInfo depInfo = {};
    depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    depInfo.pNext = nullptr;

    depInfo.imageMemoryBarrierCount = 1;
    depInfo.pImageMemoryBarriers = &imageBarrier;

    vkCmdPipelineBarrier2(cmd, &depInfo);
}

void CommandSubmitter::transitionImage(VkCommandBuffer cmd, std::shared_ptr<Image> image, VkImageLayout newLayout) {
    transitionVulkanImage(
            cmd,
            image->image, image->layout,
            newLayout
    );

    image->layout = newLayout;
}

void CommandSubmitter::shutdown() {

}

