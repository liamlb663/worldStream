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
    Size geometryImg = m_renderGraph->addImage("Geometry Pass");
    Size finalImg = m_renderGraph->addImage("Final Draw");

    Size geometryPass = m_renderGraph->createNode(
            "Geometry",
            [](){},
            {}
    );
    m_renderGraph->addImageOutput(geometryPass, {geometryImg});

    Size postFxPass = m_renderGraph->createNode(
            "Post Fx",
            [](){},
            {geometryPass}
    );
    m_renderGraph->addImageInput(postFxPass, {geometryPass});
    m_renderGraph->addImageOutput(postFxPass, {finalImg});

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

