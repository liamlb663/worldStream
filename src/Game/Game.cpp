// src/Game/Game.cpp

#include "Game.hpp"
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

    MaterialInfo* demoMaterialInfo = m_resources.getMaterialManager()->getInfo("triangle");

    MaterialData data = {
        .pipeline = demoMaterialInfo,
        .descriptors = {}
    };

    RenderObject obj = {
        .indexCount = 3,
        .startIndex = 0,
        .indexBuffer = nullptr,
        .material = &data,
    };

    std::shared_ptr<Image> img = m_resources.loadImage("clouds.png");
    m_resources.dropImage(img);

    m_input->bindAction("Quit", GLFW_KEY_Q);

    while (!m_input->shouldClose()) {
        m_input->update();

        if (m_input->isPressed("Quit"))
            m_input->close();

        m_graphics.renderObjects(0, {obj});
        m_graphics.renderFrame();
    }
}

void Game::shutdown() {
    spdlog::info("Shutting Down Game");

    delete m_input;

    m_graphics.waitOnGpu();
    m_resources.shutdown();
    m_graphics.shutdown();
}

