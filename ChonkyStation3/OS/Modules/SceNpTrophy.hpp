#pragma once

#include <common.hpp>
#include <logger.hpp>
#include <BEField.hpp>

#include <CellTypes.hpp>


// Circular dependency
class PlayStation3;

using namespace CellTypes;

class SceNpTrophy {
public:
    SceNpTrophy(PlayStation3* ps3) : ps3(ps3) {}
    PlayStation3* ps3;

    u64 sceNpTrophyCreateHandle();
    u64 sceNpTrophyCreateContext();

private:
    MAKE_LOG_FUNCTION(log, sceNpTrophy);
};