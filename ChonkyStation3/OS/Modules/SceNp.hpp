#pragma once

#include <common.hpp>
#include <logger.hpp>
#include <BEField.hpp>

#include <CellTypes.hpp>


// Circular dependency
class PlayStation3;

using namespace CellTypes;

static constexpr u64 SCE_NP_ERROR_OFFLINE = 0x8002aa0c;

class SceNp {
public:
    SceNp(PlayStation3* ps3) : ps3(ps3) {}
    PlayStation3* ps3;

    u64 sceNpManagerGetNpId();

private:
    MAKE_LOG_FUNCTION(log, sceNp);
};