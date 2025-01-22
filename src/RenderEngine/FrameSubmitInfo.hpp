// src/RenderEngine/CommandSubmitter.hpp

#pragma once

#include "Core/Types.hpp"

#include "FrameManagement/FrameData.hpp"
#include "FrameManagement/SwapchainManager.hpp"
#include "RenderEngine/RenderGraph/RenderGraph.hpp"

struct FrameSubmitInfo {
    Size frameNumber;
    FrameData frameData;
    SwapchainImage swapchainImage;
};

struct RecordInfo {
    VkCommandBuffer commandBuffer;
    RenderInfo* renderContext;
};
