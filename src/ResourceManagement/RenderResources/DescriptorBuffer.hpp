// src/ResourceManagement/RenderResources/DescriptorBuffer.hpp

#pragma once

#include "Core/Types.hpp"
#include "RenderEngine/RenderObjects/Materials.hpp"
#include "RenderEngine/VulkanInfo.hpp"
#include <vulkan/vulkan.h>

#include "Buffer.hpp"
#include "Image.hpp"

#include <cstring>
#include <vector>

struct BindingOffset {
    U32 binding;
    VkDeviceSize offset;
};

class DescriptorBuffer {
public:
    bool init(VulkanInfo* vkInfo, Size size);
    void shutdown();

    U32 allocateSlot(DescriptorSetInfo* info);
    void mapUniformBuffer(U32 setID, U32 binding, Buffer* buffer, Size range, Size offset);
    void mapImageSampler(U32 setID, U32 binding, Image* image, VkSampler sampler);

    void bindDescriptorBuffer(VkCommandBuffer commandBuffer);
    void bindDescriptorViaOffset(
        VkCommandBuffer commandBuffer,
        VkPipelineBindPoint pipelineBindPoint,
        VkPipelineLayout pipelineLayout,
        U32 setIndex,
        U32 descriptorSetID
    );

    void verify(U32 setID, U32 binding, const char* context) const;
    void verify(U32 setID, U32 binding, const char* context, Size expectedOffset, Size expectedSize) const;

private:
    VulkanInfo* m_vkInfo;
    Buffer m_buffer;

    VkDeviceSize m_currentOffset = 0;
    VkDeviceSize m_alignment = 1;

    struct SetSlot {
        DescriptorSetInfo* info;
        Size size;
        Size offset;
        std::unordered_map<U32, VkDescriptorType> bindingTypes;
        std::unordered_map<U32, VkDeviceSize> bindingOffsets;
    };

    std::vector<SetSlot> m_sets;

    VkPhysicalDeviceDescriptorBufferPropertiesEXT props;

    static PFN_vkCmdBindDescriptorBuffersEXT vkCmdBindDescriptorBuffersEXT;
    static PFN_vkCmdSetDescriptorBufferOffsetsEXT vkCmdSetDescriptorBufferOffsetsEXT;
    static PFN_vkGetDescriptorEXT vkGetDescriptorEXT;

    VkDeviceSize calculateDescriptorSize(VkDescriptorType type) const;
};

