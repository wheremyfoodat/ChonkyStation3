#pragma once

#include <common.hpp>
#include <logger.hpp>
#include <CellTypes.hpp>
#include <BEField.hpp>


// Circular dependency
class PlayStation3;

using namespace CellTypes;

class CellResc {
public:
    CellResc(PlayStation3* ps3) : ps3(ps3) {}
    PlayStation3* ps3;
    MAKE_LOG_FUNCTION(log, cellResc);

    u64 cellRescInit();
    u64 cellRescVideoOutResolutionId2RescBufferMode();
};