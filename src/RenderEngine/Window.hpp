// src/RenderEngine/Window.hpp

#pragma once

#include "Core/Vector.hpp"

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

class Window {
public:
    Window(uint32_t width, uint32_t height, const std::string& title);
    Window(Vector<uint32_t, 2> size, const std::string& title);

    bool init(VkInstance instance);
    void shutdown(VkInstance instance);

    GLFWwindow* getGLFWwindow() const;
    Vector<uint32_t, 2> getSize() const;

    VkSurfaceKHR getSurface() const;
    void initVkSurface(VkInstance instance, GLFWwindow* window);

private:

    Vector<uint32_t, 2> m_size;
    std::string m_title;

    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    GLFWwindow* m_window = nullptr;

};
