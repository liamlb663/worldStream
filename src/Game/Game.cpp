// src/Game/Game.hpp

#include "Game.hpp"
#include "spdlog/spdlog.h"

bool Game::initialize(int argc, char* argv[]) {
    spdlog::info("Initializing Game");
    m_graphics.initialize();

    (void) argc;
    (void) argv;

    return true;
}

void Game::run() {
    spdlog::info("Running Game");

}

void Game::shutdown() {
    spdlog::info("Shutting Down Game");
    m_graphics.shutdown();
}

