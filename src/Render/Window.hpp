// src/Render/Window.hpp

#pragma once

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

class Window {
public:
    Window(uint32_t width, uint32_t height, const std::string& title);
    Window(VkExtent2D extent, const std::string& title);
    ~Window();

    GLFWwindow* getGLFWwindow() const;
    VkExtent2D getExtent();

    VkSurfaceKHR getSurface();

private:
    void init();

    VkSurfaceKHR m_surface;
    GLFWwindow* m_window;
    VkExtent2D m_extent;
    std::string m_title;
};
