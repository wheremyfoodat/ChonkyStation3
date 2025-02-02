#pragma once

#include <common.hpp>
#include <logger.hpp>
#include <BEField.hpp>

#include <CellTypes.hpp>


// Circular dependency
class PlayStation3;

using namespace CellTypes;

static constexpr u64 CELL_DISCGAME_ERROR_NOT_DISCBOOT = 0x8002bd02;

class CellGame {
public:
    CellGame(PlayStation3* ps3) : ps3(ps3) {}
    PlayStation3* ps3;

    enum CellGameType : u32 {
        CELL_GAME_GAMETYPE_SYS,
        CELL_GAME_GAMETYPE_DISC,
        CELL_GAME_GAMETYPE_HDD,
        CELL_GAME_GAMETYPE_GAMEDATA,
        CELL_GAME_GAMETYPE_HOME,
    };

    std::string content_path = "/dev_hdd0/game/STUB12345\0\0";
    void setContentPath(fs::path path);

    u64 cellGameContentPermit();
    u64 cellGameBootCheck();

    u64 cellDiscGameGetBootDiscInfo();

private:
    MAKE_LOG_FUNCTION(log, cellGame);
};