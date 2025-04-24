// src/ResourceManagement/RenderResources/Sampler.hpp

#pragma once

#include "RenderEngine/VulkanInfo.hpp"
#include <vulkan/vulkan.h>
#include <memory>
#include <expected>

// --- Sampler ---

class Sampler {
public:
    Sampler(VulkanInfo* vkInfo, VkSampler sampler)
        : m_vkInfo(vkInfo), m_sampler(sampler) {}

    VkSampler get() const;
    void shutdown();

private:
    VulkanInfo* m_vkInfo = nullptr;
    VkSampler m_sampler;
};

// --- SamplerBuilder ---

class SamplerBuilder {
public:
    SamplerBuilder(VulkanInfo* vkInfo);

    SamplerBuilder& setFilter(VkFilter magFilter, VkFilter minFilter);
    SamplerBuilder& setAddressMode(VkSamplerAddressMode modeU, VkSamplerAddressMode modeV, VkSamplerAddressMode modeW);
    SamplerBuilder& enableAnisotropy(float maxAnisotropy = 16.0f);
    SamplerBuilder& disableAnisotropy();
    SamplerBuilder& setLod(float minLod, float maxLod);
    SamplerBuilder& setBorderColor(VkBorderColor color);
    SamplerBuilder& setCompareOp(VkCompareOp compareOp);
    SamplerBuilder& setUnnormalizedCoords(bool unnormalized);

    std::expected<Sampler, U32> build();

private:
    VulkanInfo* m_vkInfo = nullptr;
    VkSamplerCreateInfo m_info;
};
