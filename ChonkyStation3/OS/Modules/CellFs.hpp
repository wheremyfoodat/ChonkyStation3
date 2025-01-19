#pragma once

#include <common.hpp>
#include <logger.hpp>
#include <BEField.hpp>

#include <CellTypes.hpp>

#include <unordered_map>


// Circular dependency
class PlayStation3;

using namespace CellTypes;

class CellFs {
public:
    CellFs(PlayStation3* ps3) : ps3(ps3) {}
    PlayStation3* ps3;

    u64 cellFsClose();
    u64 cellFsRead();
    u64 cellFsOpen();
    u64 cellFsStat();
    u64 cellFsLseek();

private:
    MAKE_LOG_FUNCTION(log, cellFs);
};