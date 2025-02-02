// src/RenderEngine/FrameData.hpp

#pragma once

#include "Core/DeletionQueue.hpp"
#include "Core/Types.hpp"
#include "Core/Vector.hpp"

#include "../VulkanInfo.hpp"

#include "../InternalResources/CommandPool.hpp"
#include "../InternalResources/Fence.hpp"
#include "../InternalResources/Semaphore.hpp"
#include "../RenderGraph/RenderGraph.hpp"
#include "../RenderObjects/RenderObject.hpp"

#include <memory>
#include <vulkan/vulkan.h>

class FrameData {
public:
    bool init(std::shared_ptr<VulkanInfo> vkInfo, Vector<U32, 2> size, Size frameNumber);
    void shutdown();

    bool regenerate(Vector<U32, 2> size);
    void changeRenderGraph(std::shared_ptr<RenderGraph> renderGraph);

    void addRenderObjects(Size geoId, std::vector<RenderObject> objects);

    CommandPool commandPool;
    VkCommandBuffer transferBuffer;

    Semaphore swapchainSemaphore;
    Semaphore renderSemaphore;
    Fence renderFence;

    DeletionQueue deletionQueue;

    // Render Resources
    RenderInfo renderContext;
    std::shared_ptr<RenderGraph> renderGraph;

private:
    std::shared_ptr<VulkanInfo> m_vkInfo;
    Size m_frameNumber;
    Vector<U32, 2> m_currentWindowSize;

};

