// src/ResourceManagement/RenderResources/DescriptorSet.hpp

#pragma once

#include "Buffer.hpp"
#include "Core/Types.hpp"
#include "RenderEngine/VulkanInfo.hpp"

#include <cstring>
#include <memory>
#include <vector>

class DescriptorSet {
public:
    bool init(std::shared_ptr<VulkanInfo> vkInfo, Size size);
    void shutdown();

    void addBinding(Size size);
    std::vector<VkDescriptorBufferBindingInfoEXT> getBindingInfo() const;

    Size getSize() const;
    void update(void* data, Size size, Size offset);

private:
    std::shared_ptr<VulkanInfo> m_vkInfo;
    Buffer m_buffer;
    std::vector<Size> m_bindings;
    VkDeviceAddress m_address = 0;

    VkDeviceAddress getDeviceAddress() const;
};
