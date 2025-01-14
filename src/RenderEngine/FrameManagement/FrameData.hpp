// src/RenderEngine/FrameData.hpp

#pragma once

#include "Core/DeletionQueue.hpp"
#include "Core/Types.hpp"

#include "../VulkanInfo.hpp"

#include "../InternalResources/CommandPool.hpp"
#include "../InternalResources/Fence.hpp"
#include "../InternalResources/Semaphore.hpp"

#include "RenderEngine/RenderGraph/RenderGraph.hpp"
#include "ResourceManagement/RenderResources/Image.hpp"

#include <memory>
#include <vulkan/vulkan.h>

class FrameData {
public:
    bool init(std::shared_ptr<VulkanInfo> vkInfo, Vector<U32, 2> size, Size frameNumber);
    void shutdown();

    bool regenerate(Vector<U32, 2> size);

    CommandPool commandPool;
    VkCommandBuffer transferBuffer;

    Semaphore swapchainSemaphore;
    Semaphore renderSemaphore;
    Fence renderFence;

    DeletionQueue deletionQueue;

    Image drawImage;
    Image depthImage;

    std::shared_ptr<RenderGraph> renderGraph;

private:
    bool createImages(Vector<U32, 2> size);

    std::shared_ptr<VulkanInfo> m_vkInfo;
    Size m_frameNumber;
};

