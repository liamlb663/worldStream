// src/ResourceManagement/RenderResources/DescriptorPool.cpp

#include "DescriptorPool.hpp"
#include "RenderEngine/VkUtils.hpp"
#include "spdlog/spdlog.h"
#include "DescriptorSet.hpp"

bool DescriptorPool::init(VulkanInfo* vkInfo, uint32_t initialSetsPerPool, std::span<PoolSizeRatio> poolRatios) {
    m_vkInfo = vkInfo;

    ratios.clear();
    for (PoolSizeRatio& ratio : poolRatios) {
        ratios.push_back(ratio);
    }

    VkDescriptorPool newPool = createPool(initialSetsPerPool, ratios);

    // Grow setsPerPool for next call
    setsPerPool = initialSetsPerPool * 1.5;

    readyPools.push_back(newPool);

    return true;
}

void DescriptorPool::clearPools() {
    for (auto pool : readyPools) {
        vkResetDescriptorPool(m_vkInfo->device, pool, 0);
    }

    for (auto pool : fullPools) {
        vkResetDescriptorPool(m_vkInfo->device, pool, 0);
        readyPools.push_back(pool);
    }

    fullPools.clear();
}

void DescriptorPool::destroyPools() {
    for (auto pool : readyPools) {
        vkDestroyDescriptorPool(m_vkInfo->device, pool, nullptr);
    }
    readyPools.clear();

    for (auto pool : fullPools) {
        vkDestroyDescriptorPool(m_vkInfo->device, pool, nullptr);
    }
    fullPools.clear();
}

DescriptorSet DescriptorPool::allocate(VkDescriptorSetLayout layout) {
    VkDescriptorPool pool = getPool();

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;

    allocInfo.descriptorPool = pool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;

    VkDescriptorSet descriptorSet;
    auto res = vkAllocateDescriptorSets(m_vkInfo->device, &allocInfo, &descriptorSet);

    if (res == VK_ERROR_OUT_OF_POOL_MEMORY || res == VK_ERROR_FRAGMENTED_POOL) {
        fullPools.push_back(pool);

        pool = getPool();
        allocInfo.descriptorPool = pool;

        res = vkAllocateDescriptorSets(m_vkInfo->device, &allocInfo, &descriptorSet);
        if (!VkUtils::checkVkResult(res, "Could not allocate Descriptor Set")) {
            spdlog::error("This error has not been accounted for!");
        }
    }

    readyPools.push_back(pool);

    DescriptorSet outputSet = {};
    outputSet.init(m_vkInfo, descriptorSet);
    return outputSet;
}

VkDescriptorPool DescriptorPool::getPool() {
    VkDescriptorPool output;

    if (!readyPools.empty()) {
        output = readyPools.back();
        readyPools.pop_back();
    } else {

        output = createPool(setsPerPool, ratios);
        setsPerPool *= 1.5;
        setsPerPool = std::min((uint32_t)4092, setsPerPool);
    }

    return output;
}

VkDescriptorPool DescriptorPool::createPool(uint32_t setCount, std::span<PoolSizeRatio> poolRatios) {
    std::vector<VkDescriptorPoolSize> poolSizes;
    for (PoolSizeRatio ratio : poolRatios) {

        VkDescriptorPoolSize poolSize;
        poolSize.type = ratio.type;
        poolSize.descriptorCount = uint32_t(ratio.ratio * setCount);

        poolSizes.push_back(poolSize);
    }

    VkDescriptorPoolCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;

    info.maxSets = setCount;
    info.pPoolSizes = poolSizes.data();
    info.poolSizeCount = static_cast<U32>(poolSizes.size());

    VkDescriptorPool pool;
    auto res = vkCreateDescriptorPool(m_vkInfo->device, &info, nullptr, &pool);
    if (!VkUtils::checkVkResult(res, "Failed to create descriptor pool")) {
        spdlog::error("This error has not been accounted for!");
    }

    return pool;
}

