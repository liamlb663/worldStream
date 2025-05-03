// src/Game/Game.cpp

#include "Game.hpp"
#include "Game/Input/Duration.hpp"
#include "Game/Scene/TestScene.hpp"
#include "imgui.h"

#include <spdlog/spdlog.h>
#include <vulkan/vulkan.h>

#include <unistd.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

bool Game::initialize(int argc, char* argv[]) {
    (void) argc;
    (void) argv;

    spdlog::info("Initializing Game");

    if (!m_graphics.initialize()) return false;
    if (!m_resources.initialize(m_graphics.getInfo(), m_graphics.getSubmitter())) return false;

    m_input = Input::create(m_graphics.getGLFWwindow());

    return true;
}

void Game::run() {
    spdlog::info("Running Game Engine");

    // Bind Actions
    m_input->bindAction("Quit", GLFW_KEY_Q);
    m_input->update();

    TestScene scene;
    scene.Setup(&m_resources, m_input, &m_graphics);

    Duration::TimePoint start = Duration::now();
    while (!m_input->shouldClose()) {
        m_input->update();
        float time = Duration::since(start).asSeconds();

        // Quiter!
        if (m_input->isPressed("Quit"))
            m_input->close();

        // ImGui Start
        m_graphics.StartImGui();
        ImGui::NewFrame();

        // ImGui Stats
        ImGui::Begin("Engine Level");
        ImGui::Text("DT: %f", m_input->deltaTime().asSeconds());
        ImGui::Text("FPS: %f", 1.0/m_input->deltaTime().asSeconds());
        ImGui::Text("Time: %f", time);
        ImGui::End();

        // Render Scene
        scene.Run(m_input);
        scene.Draw(&m_graphics);

        // End Render and Submit
        ImGui::Render();
        m_graphics.renderFrame();
    }

    m_graphics.waitOnGpu();
    scene.Cleanup();
}

void Game::shutdown() {
    spdlog::info("Shutting Down Game");

    delete m_input;

    m_graphics.waitOnGpu();

    m_resources.shutdown();
    m_graphics.shutdown();
}

