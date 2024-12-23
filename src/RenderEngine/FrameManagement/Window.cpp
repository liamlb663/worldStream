// src/RenderEngine/Window.cpp

#include "Window.hpp"
#include "Core/Vector.hpp"
#include "GLFW/glfw3.h"
#include "spdlog/spdlog.h"
#include <vulkan/vulkan_core.h>

Window::Window(uint32_t width, uint32_t height, const std::string& title)
    : Window(
        Vector<uint32_t, 2>(
            width,
            height
        ),
        title
    ) {}

Window::Window(Vector<uint32_t, 2> size, const std::string& title) :
    m_size(size),
    m_title(title) {
}

bool Window::init(VkInstance instance) {
    if (!glfwInit()) {
        spdlog::error("Failed to initialize GLFW");
        return false;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_window = glfwCreateWindow(m_size.value.x, m_size.value.y, m_title.c_str(), nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        spdlog::error("Failed to create GLFW window");
        return false;
    }


    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkResult err = glfwCreateWindowSurface(instance, m_window, nullptr, &surface);
    if (err) {
        spdlog::error("Error with VkSurfaceKHR creation");

        const char* error_msg;
        int ret = glfwGetError(&error_msg);

        if (ret != 0) {
            if (!error_msg)
                spdlog::error("{}", ret);
            else
                spdlog::error("{} {}", ret, error_msg);
        }

        surface = VK_NULL_HANDLE;
        return false;
    }

    m_surface = surface;

    return true;
}

void Window::shutdown(VkInstance instance) {
    if (m_surface) {
        vkDestroySurfaceKHR(instance, m_surface, nullptr);
    }

    if (m_window) {
        glfwDestroyWindow(m_window);
    }

    glfwTerminate();
}

GLFWwindow* Window::getGLFWwindow() const {
    if (!m_window) spdlog::error("GLFW Window null at time of procurement");
    return m_window;
}

Vector<U32, 2> Window::getSize() const {
    if (!m_window) spdlog::error("Size null at time of procurement");
    return m_size;
}

VkSurfaceKHR Window::getSurface() const {
    if (!m_window) spdlog::error("VkSurfaceKHR null at time of procurement");
    return m_surface;
}

