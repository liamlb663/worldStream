// src/RenderEngine/RenderEngine.hpp

#pragma once

#include "Core/DeletionQueue.hpp"
#include "FrameManagement/FrameManager.hpp"
#include "CommandSubmitter.hpp"
#include "RenderEngine/RenderObjects/TextureRenderObject.hpp"
#include "RenderGraph/RenderGraph.hpp"
#include "RenderObjects/RenderObject.hpp"
#include "VulkanInfo.hpp"

#include <memory>
#include <vulkan/vulkan.h>

class RenderEngine {
public:
    bool initialize();
    void shutdown();

    VulkanInfo* getInfo() const;
    std::shared_ptr<CommandSubmitter> getSubmitter() const;
    GLFWwindow* getGLFWwindow() const { return m_frameManager->getGLFWwindow(); };

    void StartImGui();
    void renderObjects(Size geoId, std::vector<RenderObject> objects);
    void renderTextureObjects(std::vector<TextureRenderObject> objects);
    void setRenderGraph(std::shared_ptr<RenderGraph> graph);
    void renderFrame();

    void waitOnGpu();

private:
    bool initVulkan();
    bool initFramedata();
    bool initImGui();

    VulkanInfo m_vkInfo;

    std::shared_ptr<FrameManager> m_frameManager;
    std::shared_ptr<CommandSubmitter> m_commandSubmitter;

    DeletionQueue m_mainDeletionQueue;

};

