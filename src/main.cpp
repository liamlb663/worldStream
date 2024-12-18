#include <fmt/core.h>
#include <spdlog/spdlog.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

int main() {
    fmt::print("Hello from fmt!\n");
    spdlog::info("Hello from spdlog!");

    // Initialize GLFW
    if (!glfwInit()) {
        spdlog::error("Failed to initialize GLFW!");
        return -1;
    }

    spdlog::info("GLFW initialized successfully!");

    // Set up GLFW for Vulkan
    if (!glfwVulkanSupported()) {
        spdlog::error("Vulkan is not supported on this system!");
        glfwTerminate();
        return -1;
    }

    spdlog::info("Vulkan is supported!");

    // Vulkan instance creation
    VkInstance instance;
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

    // Check validation layers
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    spdlog::info("Available validation layers: {}", layerCount);

    // Application info
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan Test App";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    createInfo.pApplicationInfo = &appInfo;

    // Create Vulkan instance
    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        spdlog::error("Failed to create Vulkan instance!");
        glfwTerminate();
        return -1;
    }

    spdlog::info("Vulkan instance created successfully!");

    // Clean up Vulkan
    vkDestroyInstance(instance, nullptr);

    // Terminate GLFW
    glfwTerminate();
    spdlog::info("GLFW terminated successfully!");

    return 0;
}

