// src/RenderEngine/CommandSubmitter.hpp

#pragma once

#include "Core/Types.hpp"

#include "FrameManagement/FrameData.hpp"
#include "FrameManagement/SwapchainManager.hpp"
#include "RenderGraph/RenderGraph.hpp"
class CommandSubmitter;

struct FrameSubmitInfo {
    Size frameNumber;
    FrameData frameData;
    SwapchainImage swapchainImage;
};

struct RecordInfo {
    VkCommandBuffer commandBuffer;
    RenderInfo* renderContext;
    SwapchainImage* swapchainImage;
    CommandSubmitter* commandSubmitter;
};
