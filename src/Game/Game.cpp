// src/Game/Game.cpp

#include "Game.hpp"
#include "AssetManagement/Meshes/Mesh.hpp"
#include "AssetManagement/Meshes/PlaneGenerator.hpp"
#include "Game/RenderGraphSetup.hpp"

#include "RenderEngine/RenderObjects/Materials.hpp"

#include <spdlog/spdlog.h>
#include <vulkan/vulkan.h>

#include <unistd.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstring>  // for memcpy

bool Game::initialize(int argc, char* argv[]) {
    (void) argc;
    (void) argv;

    spdlog::info("Initializing Game");

    m_graphics.initialize();
    m_resources.initialize(m_graphics.getInfo(), m_graphics.getSubmitter());
    m_input = Input::create(m_graphics.getGLFWwindow());

    m_renderGraph = setupRenderGraph();

    m_graphics.setRenderGraph(m_renderGraph);

    return true;
}

void Game::run() {
    spdlog::info("Running Game");

    plane = createPlane(&m_resources, "mesh");

// Default Model Matrix: identity (no transform)
glm::mat4 modelMatrix = glm::mat4(1.0f);

// Default View Matrix: look from Z+ toward origin
glm::mat4 viewMatrix = glm::lookAt(
    glm::vec3(0.0f, 0.0f, -2.0f),  // camera position
    glm::vec3(0.0f, 0.0f, 0.0f),  // target
    glm::vec3(0.0f, 0.0f, 1.0f)   // up vector
);

// Default Projection Matrix: 45° perspective, right-handed
glm::mat4 projMatrix = glm::perspective(
    glm::radians(45.0f),
    1.0f,      // aspect ratio (square viewport for now)
    0.1f,      // near clip
    10.0f      // far clip
);

// Vulkan-style projection fix: invert Y for GLM to match Vulkan clip space
projMatrix[1][1] *= -1.0f;

// Pointer to mapped data
void* mappedData = plane.materialBuffer.info.pMappedData;

// Cast to float* for direct matrix copy
float* dst = static_cast<float*>(mappedData);

// Copy model, view, and projection matrices (16 floats each)
memcpy(dst,               &modelMatrix, sizeof(glm::mat4));
memcpy(dst + 16,          &viewMatrix,  sizeof(glm::mat4));
memcpy(dst + 16 + 16,     &projMatrix,  sizeof(glm::mat4));
/*
    plane.descriptor.
        verify(0, 0, "from mesh");

    plane.draw()[0].material->descriptors[0].buffer->
        verify(0, 0, "from render Object");
*/

    m_input->bindAction("Quit", GLFW_KEY_Q);

    m_graphics.renderObjects(0, plane.draw());
    m_graphics.renderFrame();

    while (!m_input->shouldClose()) {
        m_input->update();

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

