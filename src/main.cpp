
#include "Game/Game.hpp"
#include "spdlog/spdlog.h"

int main(int argc, char* argv[]) {
    spdlog::set_level(spdlog::level::trace);
    Game game;

    if (!game.initialize(argc, argv)) {
        spdlog::error("Initialization failed!");
        return -1;
    }

    game.run();

    game.shutdown();

    return 0;
}

