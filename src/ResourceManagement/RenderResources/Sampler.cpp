// src/ResourceManagement/RenderResources/Sampler.cpp

#include "Sampler.hpp"

// --- Sampler ---

VkSampler Sampler::get() const {
    return m_sampler;
}

void Sampler::shutdown() {
    vkDestroySampler(m_vkInfo->device, m_sampler, nullptr);

    m_sampler = VK_NULL_HANDLE;
    m_vkInfo = nullptr;
}

// --- SamplerBuilder ---

SamplerBuilder::SamplerBuilder(VulkanInfo* vkInfo)
    : m_vkInfo(vkInfo) {
    m_info = {};
    m_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    m_info.magFilter = VK_FILTER_LINEAR;
    m_info.minFilter = VK_FILTER_LINEAR;
    m_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    m_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    m_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    m_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    m_info.mipLodBias = 0.0f;
    m_info.anisotropyEnable = VK_FALSE;
    m_info.maxAnisotropy = 1.0f;
    m_info.compareEnable = VK_FALSE;
    m_info.compareOp = VK_COMPARE_OP_ALWAYS;
    m_info.minLod = 0.0f;
    m_info.maxLod = VK_LOD_CLAMP_NONE;
    m_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    m_info.unnormalizedCoordinates = VK_FALSE;
}

SamplerBuilder& SamplerBuilder::setFilter(VkFilter magFilter, VkFilter minFilter) {
    m_info.magFilter = magFilter;
    m_info.minFilter = minFilter;
    return *this;
}

SamplerBuilder& SamplerBuilder::setAddressMode(VkSamplerAddressMode modeU, VkSamplerAddressMode modeV, VkSamplerAddressMode modeW) {
    m_info.addressModeU = modeU;
    m_info.addressModeV = modeV;
    m_info.addressModeW = modeW;
    return *this;
}

SamplerBuilder& SamplerBuilder::enableAnisotropy(float maxAnisotropy) {
    m_info.anisotropyEnable = VK_TRUE;
    m_info.maxAnisotropy = maxAnisotropy;
    return *this;
}

SamplerBuilder& SamplerBuilder::disableAnisotropy() {
    m_info.anisotropyEnable = VK_FALSE;
    m_info.maxAnisotropy = 1.0f;
    return *this;
}

SamplerBuilder& SamplerBuilder::setLod(float minLod, float maxLod) {
    m_info.minLod = minLod;
    m_info.maxLod = maxLod;
    return *this;
}

SamplerBuilder& SamplerBuilder::setBorderColor(VkBorderColor color) {
    m_info.borderColor = color;
    return *this;
}

SamplerBuilder& SamplerBuilder::setCompareOp(VkCompareOp compareOp) {
    m_info.compareEnable = VK_TRUE;
    m_info.compareOp = compareOp;
    return *this;
}

SamplerBuilder& SamplerBuilder::setUnnormalizedCoords(bool unnormalized) {
    m_info.unnormalizedCoordinates = unnormalized ? VK_TRUE : VK_FALSE;
    return *this;
}

std::expected<Sampler, U32> SamplerBuilder::build() {
    VkSampler sampler;
    if (vkCreateSampler(m_vkInfo->device, &m_info, nullptr, &sampler) != VK_SUCCESS) {
        return std::unexpected(1);  // error code
    }
    return Sampler(m_vkInfo, sampler);
}
