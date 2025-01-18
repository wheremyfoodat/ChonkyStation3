#pragma once

#include <common.hpp>
#include <logger.hpp>
#include <BEField.hpp>

#include <CellTypes.hpp>


// Circular dependency
class PlayStation3;

using namespace CellTypes;

class CellRtc {
public:
    CellRtc(PlayStation3* ps3) : ps3(ps3) {}
    PlayStation3* ps3;

    u64 cellRtcGetCurrentTick();

private:
    MAKE_LOG_FUNCTION(log, cellRtc);
};