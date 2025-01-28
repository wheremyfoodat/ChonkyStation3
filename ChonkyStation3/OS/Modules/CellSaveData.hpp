#pragma once

#include <common.hpp>
#include <logger.hpp>
#include <BEField.hpp>

#include <CellTypes.hpp>


// Circular dependency
class PlayStation3;

using namespace CellTypes;

class CellSaveData {
public:
    CellSaveData(PlayStation3* ps3) : ps3(ps3) {}
    PlayStation3* ps3;

    u64 cellSaveDataAutoSave2();
    u64 cellSaveDataAutoLoad2();

private:
    MAKE_LOG_FUNCTION(log, cellSaveData);
};