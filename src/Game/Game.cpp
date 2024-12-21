// src/Game/Game.hpp

#include "Game.hpp"

bool Game::initialize(int argc, char* argv[]) {
    m_graphics.initialize();

    (void) argc;
    (void) argv;

    return true;
}

void Game::run() {

}

void Game::shutdown() {
    m_graphics.shutdown();
}

