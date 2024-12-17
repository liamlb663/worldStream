#include <fmt/core.h>
#include <spdlog/spdlog.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <glm/glm.hpp>

int main() {
    fmt::print("Hello from fmt!\n");
    spdlog::info("Hello from spdlog!");

    if (!glfwInit()) {
        spdlog::error("Failed to initialize GLFW!");
        return -1;
    }

    spdlog::info("GLFW initialized successfully!");

    glfwTerminate();
    return 0;
}
