// src/RenderEngine/RenderEngine.hpp

#include "RenderEngine.hpp"

#include "Config.hpp"
#include "Debug.hpp"
#include "InternalResources/CommandPool.hpp"
#include "RenderEngine/FrameSubmitInfo.hpp"
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
    //Create Instance + debug
    vkb::InstanceBuilder builder;

    auto instanceReturn = builder.set_app_name("World Streaming Engine")
        .add_validation_feature_enable(VkValidationFeatureEnableEXT::VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT)
        //.add_validation_feature_enable(VkValidationFeatureEnableEXT::VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT)
        .request_validation_layers(Config::useValidationLayers)
        .set_debug_callback(Debug::CustomDebugCallback)
        .require_api_version(1, 3, 0)
        .build();

    if (!instanceReturn) {
        spdlog::error("Could not build vulkan instance, {}", instanceReturn.error().message());
        return false;
    }
    vkb::Instance vkbInstance = instanceReturn.value();

    m_vkInfo.instance = vkbInstance.instance;
    m_vkInfo.debugMessenger = vkbInstance.debug_messenger;

    // Load Vulkan debug functions
    Debug::LoadDebugUtils(m_vkInfo.instance);

    m_frameManager = std::make_shared<FrameManager>();
    m_frameManager->initializeWindow(&m_vkInfo);

    //Pick and Create Devices
    VkPhysicalDeviceDescriptorBufferFeaturesEXT descriptorBufferFeatures{};
    descriptorBufferFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_FEATURES_EXT;
    descriptorBufferFeatures.descriptorBuffer = VK_TRUE;

    VkPhysicalDeviceVulkan13Features features13{};
    features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    features13.pNext = &descriptorBufferFeatures;
    features13.dynamicRendering = VK_TRUE;
    features13.synchronization2 = VK_TRUE;

    VkPhysicalDeviceVulkan12Features features12{};
    features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    features12.pNext = &features13;
    features12.bufferDeviceAddress = VK_TRUE;
    features12.descriptorIndexing = VK_TRUE;

    VkPhysicalDeviceFeatures features10{};
    features10.samplerAnisotropy = VK_TRUE;
    features10.shaderSampledImageArrayDynamicIndexing = VK_TRUE;
    features10.sparseBinding = VK_TRUE;
    features10.sparseResidencyBuffer = VK_TRUE;

    vkb::PhysicalDeviceSelector selector = vkb::PhysicalDeviceSelector(vkbInstance);

    auto physicalDeviceReturn = selector
        .set_minimum_version(1, 3)
        .set_required_features_12(features12)
        .set_required_features(features10)
        .add_required_extension("VK_EXT_descriptor_buffer")
        .disable_portability_subset()
        .set_surface(m_frameManager->getWindow()->getSurface())
        .select();

    if (!physicalDeviceReturn) {
        spdlog::error("Failed to select Vulkan physical device. Error: {}",
                physicalDeviceReturn.error().message());

        return false;
    }

    vkb::PhysicalDevice physicalDevice = physicalDeviceReturn.value();
    vkb::DeviceBuilder deviceBuilder = vkb::DeviceBuilder(physicalDevice);

    auto deviceReturn = deviceBuilder.build();
    if (!deviceReturn) {
        spdlog::error("Failed to create Vulkan device. Error: ",
                deviceReturn.error().message());

        return false;
    }
    vkb::Device vkbDevice = deviceReturn.value();

    m_vkInfo.device = vkbDevice.device;
    m_vkInfo.physicalDevice = physicalDevice.physical_device;

    // Graphics Queue
    auto graphicsQueueReturn = vkbDevice.get_queue(vkb::QueueType::graphics);
    if (!graphicsQueueReturn) {
        spdlog::error("Failed to get graphics queue, Error: {}",
                graphicsQueueReturn.error().message());

        return false;
    }
    m_vkInfo.graphicsQueue = graphicsQueueReturn.value();

    auto graphicsQueueFamilyReturn = vkbDevice.get_queue_index(vkb::QueueType::graphics);
    if (!graphicsQueueFamilyReturn) {
        spdlog::error("Failed to get graphics queue family, Error: {}",
                graphicsQueueFamilyReturn.error().message());

        return false;
    }
    m_vkInfo.graphicsQueueFamily = graphicsQueueFamilyReturn.value();

    // Transfer Queue
    auto transferQueueReturn = vkbDevice.get_queue(vkb::QueueType::transfer);
    if (!transferQueueReturn) {
        spdlog::error("Failed to get transfer queue, Error: {}",
                transferQueueReturn.error().message());

        return false;
    }
    m_vkInfo.transferQueue = transferQueueReturn.value();

    auto transferQueueFamilyReturn = vkbDevice.get_queue_index(vkb::QueueType::transfer);
    if (!transferQueueFamilyReturn) {
        spdlog::error("Failed to get transfer queue family, Error: {}",
                transferQueueFamilyReturn.error().message());

        return false;
    }
    m_vkInfo.transferQueueFamily = transferQueueFamilyReturn.value();

    m_mainDeletionQueue.push([this]() {
            vkDestroyDevice(m_vkInfo.device, nullptr);
            vkb::destroy_debug_utils_messenger(m_vkInfo.instance, m_vkInfo.debugMessenger);
            vkDestroyInstance(m_vkInfo.instance, nullptr);
    });

    // Create VMA
    VmaAllocatorCreateInfo vmaInfo{};
    vmaInfo.physicalDevice = m_vkInfo.physicalDevice;
    vmaInfo.device = m_vkInfo.device;
    vmaInfo.instance = m_vkInfo.instance;
    vmaInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

    auto res = vmaCreateAllocator(&vmaInfo, &m_vkInfo.allocator);
    if (!VkUtils::checkVkResult(res, "Failed to create vma allocator")) return false;

    m_mainDeletionQueue.push([this]() {
        VmaTotalStatistics stats;
        vmaCalculateStatistics(m_vkInfo.allocator, &stats);

        Size bytesLeft = stats.total.statistics.allocationBytes;

        if (bytesLeft > 0) {
            spdlog::error("Attempted to destroy VmaAllocator with allocated bytes");
            spdlog::error("{} bytes left allocated not destroying VmaAllocator", bytesLeft);
        } else {
            vmaDestroyAllocator(m_vkInfo.allocator);
        }
    });

    // Create Transfer Pool
    m_vkInfo.transferPool = new CommandPool();
    m_vkInfo.transferPool->initialize(
        &m_vkInfo, CommandPoolType::Transfer,
        VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        "Transfer Pool"
    );

    m_mainDeletionQueue.push([this]() {
        m_vkInfo.transferPool->shutdown();
        delete m_vkInfo.transferPool;
    });

    m_commandSubmitter = std::make_shared<CommandSubmitter>();
    if (!m_commandSubmitter->initialize(&m_vkInfo)) {
        spdlog::error("Failed to initialze CommandSubmitter");
        return false;
    }

    m_mainDeletionQueue.push([this]() {
        m_commandSubmitter->shutdown();
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

