// src/Game/Game.cpp

#include "Game.hpp"
#include "spdlog/spdlog.h"

bool Game::initialize(int argc, char* argv[]) {
    spdlog::info("Initializing Game");

    m_graphics.initialize();
    m_resources.initialize(m_graphics.getInfo(), m_graphics.getSubmitter());

    (void) argc;
    (void) argv;

    m_renderGraph = std::make_shared<RenderGraph>();
    Size finalDraw = m_renderGraph->addImage("Final Draw", glm::vec2(1.0f));
    Size postFx = m_renderGraph->addImage("Post Fx", glm::vec2(1.0f));

    Size node1 = m_renderGraph->createNode(
            "Geometry",
            [](){},
            {},
            {finalDraw},
            nullptr,
            {}
    );

    m_renderGraph->createNode(
            "Post Fx",
            [](){},
            {finalDraw},
            {postFx},
            nullptr,
            {node1}
    );

    m_renderGraph->printGraph();


    return true;
}

void Game::run() {
    spdlog::info("Running Game");

    std::shared_ptr<Image> img = m_resources.loadImage("clouds.png");

    m_resources.dropImage(img);
}

void Game::shutdown() {
    spdlog::info("Shutting Down Game");

    m_resources.shutdown();
    m_graphics.shutdown();
}

