// src/RenderEngine/RenderEngine.hpp

#include "RenderEngine.hpp"

#include "Config.hpp"
#include "Debug.hpp"
#include "InternalResources/CommandPool.hpp"
#include "RenderEngine/FrameSubmitInfo.hpp"
#include "RenderEngine/VulkanInitHelpers.hpp"
#include "VkUtils.hpp"

#include <VkBootstrap.h>
#include <memory>
#include <spdlog/spdlog.h>

#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>
#include <imgui.h>

bool RenderEngine::initialize() {
    spdlog::trace("Initialize RenderEngine");

    if (!initVulkan()) return false;
    if (!initFramedata()) return false;
    if (!initImGui()) return false;

    return true;
}

bool RenderEngine::initVulkan() {
    // Create Vulkan instance
    if (!CreateVulkanInstance(&m_vkInfo.instance, Config::useValidationLayers)) {
        spdlog::error("Failed to create Vulkan instance.");
        return false;
    }

    // Setup debug messenger
    Debug::LoadDebugUtils(m_vkInfo.instance);
    SetupDebugMessenger(m_vkInfo.instance, &m_vkInfo.debugMessenger);

    // Create window surface (platform-specific)
    m_frameManager = std::make_shared<FrameManager>();
    m_frameManager->initializeWindow(&m_vkInfo); // should create surface inside
    VkSurfaceKHR surface = m_frameManager->getWindow()->getSurface();

    // Pick physical device
    U32 graphicsFamily = 0, transferFamily = 0;
    if (!PickPhysicalDevice(m_vkInfo.instance, surface, &m_vkInfo.physicalDevice, &graphicsFamily, &transferFamily)) {
        spdlog::error("Failed to select suitable physical device.");
        return false;
    }

    // Create logical device
    VkPhysicalDeviceFeatures features10{};
    VkPhysicalDeviceVulkan12Features features12{};
    VkPhysicalDeviceVulkan13Features features13{};
    VkPhysicalDeviceDescriptorBufferFeaturesEXT descriptorBufferFeatures{};

    if (!CreateLogicalDevice(
            m_vkInfo.physicalDevice,
            surface,
            &m_vkInfo.device,
            &m_vkInfo.graphicsQueue,
            &m_vkInfo.transferQueue,
            graphicsFamily,
            transferFamily,
            features10,
            features12,
            features13,
            descriptorBufferFeatures)) {
        spdlog::error("Failed to create logical device.");
        return false;
    }

    m_vkInfo.graphicsQueueFamily = graphicsFamily;
    m_vkInfo.transferQueueFamily = transferFamily;

    // Create VMA allocator
    VmaAllocatorCreateInfo allocatorInfo{};
    allocatorInfo.physicalDevice = m_vkInfo.physicalDevice;
    allocatorInfo.device = m_vkInfo.device;
    allocatorInfo.instance = m_vkInfo.instance;
    allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

    if (vmaCreateAllocator(&allocatorInfo, &m_vkInfo.allocator) != VK_SUCCESS) {
        spdlog::error("Failed to create VMA allocator.");
        return false;
    }

    // Create command pool
    m_vkInfo.transferPool = new CommandPool();
    if (m_vkInfo.transferPool->initialize(
            &m_vkInfo, CommandPoolType::Transfer,
            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            "Transfer Pool") != VK_SUCCESS) {
        spdlog::error("Failed to initialize command pool.");
        return false;
    }

    // Initialize command submitter
    m_commandSubmitter = std::make_shared<CommandSubmitter>();
    if (!m_commandSubmitter->initialize(&m_vkInfo)) {
        spdlog::error("Failed to initialize CommandSubmitter.");
        return false;
    }

    // Cleanup registration
    m_mainDeletionQueue.push([this]() {
        vkDestroyDevice(m_vkInfo.device, nullptr);
        DestroyDebugMessenger(m_vkInfo.instance, m_vkInfo.debugMessenger);
        vkDestroyInstance(m_vkInfo.instance, nullptr);
    });

    m_mainDeletionQueue.push([this]() {
        VmaTotalStatistics stats;
        vmaCalculateStatistics(m_vkInfo.allocator, &stats);
        if (stats.total.statistics.allocationBytes > 0) {
            spdlog::error("Leaked {} bytes in VMA allocator", stats.total.statistics.allocationBytes);
        } else {
            vmaDestroyAllocator(m_vkInfo.allocator);
        }
    });

    m_mainDeletionQueue.push([this]() {
        m_commandSubmitter->shutdown();
    });

    m_mainDeletionQueue.push([this]() {
        m_vkInfo.transferPool->shutdown();
        delete m_vkInfo.transferPool;
    });

    return true;
}

bool RenderEngine::initFramedata() {
    m_vkInfo.transferPool->resizeBuffers(Config::framesInFlight);

    if (!m_frameManager->initializeFrames()) {
        spdlog::error("Failed to initialize FrameManager!");
        return false;
    }

    m_mainDeletionQueue.push([this]() {
        m_frameManager->shutdown();
    });

    return true;
}

bool RenderEngine::initImGui() {
    VkDescriptorPoolSize pool_sizes[] = {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 11000;
    pool_info.poolSizeCount = static_cast<uint32_t>(std::size(pool_sizes));
    pool_info.pPoolSizes = pool_sizes;

    VkDescriptorPool imguiPool;
    VkResult createResult = vkCreateDescriptorPool(m_vkInfo.device, &pool_info, nullptr, &imguiPool);
    if (!VkUtils::checkVkResult(createResult, "Failed to create ImGui descriptor pool.")) return false;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(m_frameManager->getGLFWwindow(), true);

    ImGui_ImplVulkan_InitInfo initInfo = {};
    initInfo.Instance = m_vkInfo.instance;
    initInfo.PhysicalDevice = m_vkInfo.physicalDevice;
    initInfo.Device = m_vkInfo.device;
    initInfo.Queue = m_vkInfo.graphicsQueue;
    initInfo.MinImageCount = Config::framesInFlight;
    initInfo.ImageCount = Config::framesInFlight;
    initInfo.UseDynamicRendering = true;
    initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    initInfo.DescriptorPool = imguiPool;

    std::vector<VkFormat> colorFormats = { Config::swapchainFormat };

    VkPipelineRenderingCreateInfoKHR pipelineRenderingInfo = {};
    pipelineRenderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    pipelineRenderingInfo.pNext = nullptr;
    pipelineRenderingInfo.colorAttachmentCount = static_cast<U32>(colorFormats.size());
    pipelineRenderingInfo.pColorAttachmentFormats = colorFormats.data();
    pipelineRenderingInfo.depthAttachmentFormat = Config::depthFormat;
    pipelineRenderingInfo.viewMask = 0;

    initInfo.PipelineRenderingCreateInfo = pipelineRenderingInfo;

    ImGui_ImplVulkan_Init(&initInfo);

    ImGui::StyleColorsDark();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    m_mainDeletionQueue.push([this, imguiPool](){
        ImGui_ImplVulkan_Shutdown();
        vkDestroyDescriptorPool(m_vkInfo.device, imguiPool, nullptr);
    });

    return true;
}

VulkanInfo* RenderEngine::getInfo() const {
    return const_cast<VulkanInfo*>(&m_vkInfo);
}

std::shared_ptr<CommandSubmitter> RenderEngine::getSubmitter() const {
    return m_commandSubmitter;
}

void RenderEngine::shutdown() {
    m_mainDeletionQueue.flush();
}

void RenderEngine::StartImGui() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
}

void RenderEngine::renderObjects(Size geoId, std::vector<RenderObject> objects) {
    m_frameManager->addRenderObjects(geoId, objects);

}

void RenderEngine::setRenderGraph(std::shared_ptr<RenderGraph> graph) {
    m_frameManager->setRenderGraph(graph);
}

void RenderEngine::renderFrame() {
    FrameSubmitInfo info = m_frameManager->getNextFrameInfo();
    m_commandSubmitter->frameSubmit(info);
    m_frameManager->presentFrame(info);
}

void RenderEngine::waitOnGpu() {
    vkDeviceWaitIdle(m_vkInfo.device);
}

