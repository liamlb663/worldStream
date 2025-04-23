// src/ResourceManagement/RenderResources/DescriptorPool.hpp

#pragma once

#include "Core/Types.hpp"
#include "RenderEngine/VulkanInfo.hpp"
class DescriptorSet;

#include <span>
#include <vector>
#include <vulkan/vulkan.h>

class DescriptorPool {
public:
    struct PoolSizeRatio {
        VkDescriptorType type;
        F32 ratio;
    };

    void init(VulkanInfo* vkInfo, U32 initialSetsPerPool, std::span<PoolSizeRatio> poolRatios);
    void clearPools();
    void destroyPools();

    DescriptorSet allocate(VkDescriptorSetLayout layout);

private:
    VkDescriptorPool getPool();
    VkDescriptorPool createPool(U32 setCount, std::span<PoolSizeRatio> poolRatios);

    uint32_t setsPerPool;
    std::vector<PoolSizeRatio> ratios;

    // Stacks
    std::vector<VkDescriptorPool> fullPools;
    std::vector<VkDescriptorPool> readyPools;

    VulkanInfo* m_vkInfo = nullptr;
};

