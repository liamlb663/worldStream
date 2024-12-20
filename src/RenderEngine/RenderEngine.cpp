// src/RenderEngine/RenderEngine.hpp

#include "RenderEngine.hpp"

#include "Config.hpp"
#include "Debug.hpp"
#include "VkUtils.hpp"

#include <VkBootstrap.h>
#include <spdlog/spdlog.h>

bool RenderEngine::initialize() {
    spdlog::trace("Initialize RenderEngine");

    if (!initVulkan()) return false;
    if (!initFramedata()) return false;

    return true;
}

bool RenderEngine::initVulkan() {
    //Create Instance + debug
    vkb::InstanceBuilder builder;

    auto instanceReturn = builder.set_app_name("World Streaming Engine")
        .add_validation_feature_enable(VkValidationFeatureEnableEXT::VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT)
        .request_validation_layers(Config::useValidationLayers)
        .set_debug_callback(Debug::CustomDebugCallback)
        .require_api_version(1, 3, 0)
        .build();

    if (!instanceReturn) {
        spdlog::error("Could not build vulkan instance, {}", instanceReturn.error().message());
        throw std::runtime_error("Could not build vulkan instance");
    }
    vkb::Instance vkbInstance = instanceReturn.value();

    m_vkInfo.instance = vkbInstance.instance;
    m_vkInfo.debugMessenger = vkbInstance.debug_messenger;

    // Load Vulkan debug functions
    Debug::LoadDebugUtils(m_vkInfo.instance);

    m_window = new Window(1700, 900, "World Streamer");
    m_window->init(m_vkInfo.instance);

    //Pick and Create Devices
    VkPhysicalDeviceVulkan13Features features13{};
    features13.dynamicRendering = true;
    features13.synchronization2 = true;

    VkPhysicalDeviceVulkan12Features features12{};
    features12.bufferDeviceAddress = true;
    features12.descriptorIndexing = true;

    VkPhysicalDeviceFeatures features10{};
    features10.samplerAnisotropy = true;

    vkb::PhysicalDeviceSelector selector = vkb::PhysicalDeviceSelector(vkbInstance);

    auto physicalDeviceReturn = selector
        .set_minimum_version(1, 3)
        .set_required_features_13(features13)
        .set_required_features_12(features12)
        .set_required_features(features10)
        .set_surface(m_window->getSurface())
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

    return true;
}

bool RenderEngine::initFramedata() {

    m_frameData.resize(Config::framesInFlight);
    for (Size i = 0; i < Config::framesInFlight; i++) {
        m_frameData[i].init(m_vkInfo);
    }

    return true;
}

void RenderEngine::shutdown() {
    m_mainDeletionQueue.flush();
}
