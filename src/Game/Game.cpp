// src/Game/Game.cpp

#include "Game.hpp"
#include "AssetManagement/Meshes/Mesh.hpp"
#include "AssetManagement/Meshes/PlaneGenerator.hpp"
#include "Game/RenderGraphSetup.hpp"

#include "RenderEngine/RenderObjects/Materials.hpp"

#include <spdlog/spdlog.h>
#include <vulkan/vulkan.h>

#include <unistd.h>


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

