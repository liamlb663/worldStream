// src/Game/Game.hpp

#pragma once

#include "RenderEngine/RenderEngine.hpp"
#include "RenderEngine/RenderGraph/RenderGraph.hpp"
#include "ResourceManagement/ResourceManager.hpp"

#include <memory>

class Game {
public:
    Game() {}

    bool initialize(int argc, char* argv[]);

    void run();

    void shutdown();

private:
    RenderEngine m_graphics;
    ResourceManager m_resources;

    std::shared_ptr<RenderGraph> m_renderGraph;
};

