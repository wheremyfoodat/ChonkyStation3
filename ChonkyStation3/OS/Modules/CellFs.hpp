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

    std::unordered_map<u32, FILE*> open_files;

    u64 cellFsOpen();
    u64 cellFsStat();

private:
    MAKE_LOG_FUNCTION(log, cellFs);
};