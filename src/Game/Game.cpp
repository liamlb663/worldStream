// src/Game/Game.cpp

#include "Game.hpp"
#include "AssetManagement/Meshes/Mesh.hpp"
#include "AssetManagement/Meshes/PlaneGenerator.hpp"
#include "Game/RenderGraphSetup.hpp"
#include "imgui.h"

#include <cmath>
#include <spdlog/spdlog.h>
#include <vulkan/vulkan.h>

#include <unistd.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstring>

bool Game::initialize(int argc, char* argv[]) {
    (void) argc;
    (void) argv;

    spdlog::info("Initializing Game");

    if (!m_graphics.initialize()) return false;
    if (!m_resources.initialize(m_graphics.getInfo(), m_graphics.getSubmitter())) return false;

    m_input = Input::create(m_graphics.getGLFWwindow());

    m_renderGraph = setupRenderGraph();

    m_graphics.setRenderGraph(m_renderGraph);

    return true;
}

void Game::run() {
    spdlog::info("Running Game");

    createPlane(&m_resources, "mesh", &plane);

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::mat4 viewMatrix = glm::lookAt(
        glm::vec3(2.0f, 2.0f, 2.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );
    glm::mat4 projMatrix = glm::perspective(
        glm::radians(45.0f),
        1.0f,
        0.1f,
        10.0f
    );
    projMatrix[1][1] *= -1.0f;

    glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f);

    void* mappedData = plane.materialBuffer.info.pMappedData;

    float* matrixDst = static_cast<float*>(mappedData);
    float* offsetDst = reinterpret_cast<float*>(static_cast<uint8_t*>(mappedData) + 192);

    memcpy(matrixDst,               &modelMatrix, sizeof(glm::mat4));
    memcpy(matrixDst + 16,          &viewMatrix,  sizeof(glm::mat4));
    memcpy(matrixDst + 16 + 16,     &projMatrix,  sizeof(glm::mat4));

    memcpy(offsetDst,               &offset,  sizeof(glm::vec3));


    m_input->bindAction("Quit", GLFW_KEY_Q);
    m_input->update();

    double time = 0;

    while (!m_input->shouldClose()) {
        m_input->update();

        m_graphics.StartImGui();
        ImGui::NewFrame();

        ImGui::Begin("Cum balls dick!");
        ImGui::Text("DT: %f", m_input->deltaTime().asSeconds());
        ImGui::Text("FPS: %f", 1.0/m_input->deltaTime().asSeconds());
        ImGui::Text("Time: %f", time);
        ImGui::End();

        ImGui::Render();

        time += m_input->deltaTime().asSeconds();

        viewMatrix = glm::lookAt(
            glm::vec3(2.0f * sin(time), 2.0f * cos(time), 2.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 1.0f)
        );

        memcpy(matrixDst,               &modelMatrix, sizeof(glm::mat4));
        memcpy(matrixDst + 16,          &viewMatrix,  sizeof(glm::mat4));
        memcpy(matrixDst + 16 + 16,     &projMatrix,  sizeof(glm::mat4));

        offset.z = sin(time);
        memcpy(offsetDst,               &offset,  sizeof(glm::vec3));

        m_graphics.renderObjects(0, plane.draw());
        m_graphics.renderFrame();

        if (m_input->isPressed("Quit"))
            m_input->close();
    }
}

void Game::shutdown() {
    spdlog::info("Shutting Down Game");

    delete m_input;

    m_graphics.waitOnGpu();

    plane.destroyMesh();

    m_resources.shutdown();
    m_graphics.shutdown();
}

