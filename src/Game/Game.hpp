// src/Game/Game.hpp

#pragma once

#include "Game/Input/Input.hpp"
#include "RenderEngine/RenderEngine.hpp"
#include "ResourceManagement/ResourceManager.hpp"

class Game {
public:
    Game() {}

    bool initialize(int argc, char* argv[]);
    void run();
    void shutdown();

private:
    RenderEngine m_graphics;
    ResourceManager m_resources;
    Input* m_input;
};

