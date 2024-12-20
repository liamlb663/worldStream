// src/RenderEngine/CommandSubmitter.hpp

#pragma once

#include <vulkan/vulkan.h>

#include <functional>

class CommandSubmitter {
public:
    bool initialize();

    void immediateSubmit(const std::function<void(VkCommandBuffer)>& function);

    void shutdown();

private:

};
