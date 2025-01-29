#pragma once

#include <common.hpp>
#include <logger.hpp>

#include <SFO/SFOLoader.hpp>

#include <unordered_map>


// Circular dependency
class PlayStation3;

class GameLoader {
public:
    GameLoader(PlayStation3* ps3);
    PlayStation3* ps3;

    struct InstalledGame {
        std::string title;
        std::string id;
        fs::path content_path;
    };
    std::vector<InstalledGame> games;

private:
    MAKE_LOG_FUNCTION(log, loader_game);
};