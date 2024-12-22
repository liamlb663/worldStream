// src/RenderEngine/CommandSubmitter.cpp

#include "CommandSubmitter.hpp"
#include "../VkUtils.hpp"

bool CommandSubmitter::initialize(VulkanInfo vkInfo, std::vector<FrameData>* frames) {
    m_vkInfo = vkInfo;

    m_frames = frames;
    m_frameNumber = 0;

    return true;
}

void CommandSubmitter::transferSubmit(const std::function<void(VkCommandBuffer)>& function) {
    // Get Buffer
    VkCommandBuffer commandBuffer = m_vkInfo.transferPool->getBuffer(m_frameNumber);

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

    res = vkQueueSubmit(m_vkInfo.transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
    if (!VkUtils::checkVkResult(res, "Failed to submit transfer command buffer.")) {
        return;
    }

    // Block on transfer completion
    vkQueueWaitIdle(m_vkInfo.transferQueue);
}

void CommandSubmitter::shutdown() {

}

