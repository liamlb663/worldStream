// src/Game/Game.cpp

#include "Game.hpp"
#include "RenderEngine/Config.hpp"
#include "spdlog/spdlog.h"

bool Game::initialize(int argc, char* argv[]) {
    spdlog::info("Initializing Game");

    m_graphics.initialize();
    m_resources.initialize(m_graphics.getInfo(), m_graphics.getSubmitter());

    (void) argc;
    (void) argv;

    m_renderGraph = std::make_shared<RenderGraph>();

    VkImageUsageFlags commonFlags = 0;
    commonFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    commonFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    commonFlags |= VK_IMAGE_USAGE_STORAGE_BIT;
    commonFlags |= VK_IMAGE_USAGE_SAMPLED_BIT;

    Size geometryImg = m_renderGraph->addImage(
            Vector<U32, 2>(0),
            Vector<F32, 2>(1),
            ImageSizeType::fractional,
            Config::drawFormat,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | commonFlags,
            "Geometry Pass"
    );
    Size finalImg = m_renderGraph->addImage(
            Vector<U32, 2>(0),
            Vector<F32, 2>(1),
            ImageSizeType::fractional,
            Config::drawFormat,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | commonFlags,
            "Final Draw"
    );

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

    m_graphics.setRenderGraph(m_renderGraph);

    return true;
}

void Game::run() {
    spdlog::info("Running Game");

    std::shared_ptr<Image> img = m_resources.loadImage("clouds.png");

    m_graphics.renderFrame();

    m_resources.dropImage(img);
}

void Game::shutdown() {
    spdlog::info("Shutting Down Game");

    m_resources.shutdown();
    m_graphics.shutdown();
}

