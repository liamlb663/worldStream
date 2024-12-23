// src/RenderEngine/Window.hpp

#pragma once

#include "Core/Types.hpp"
#include "Core/Vector.hpp"

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

class Window {
public:
    Window(U32 width, U32 height, const std::string& title);
    Window(Vector<U32, 2> size, const std::string& title);

    bool init(VkInstance instance);
    void shutdown(VkInstance instance);

    GLFWwindow* getGLFWwindow() const;
    Vector<U32, 2> getSize() const;
    VkSurfaceKHR getSurface() const;

    void initVkSurface(VkInstance instance, GLFWwindow* window);

private:

    Vector<U32, 2> m_size;
    std::string m_title;

    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    GLFWwindow* m_window = nullptr;

};
