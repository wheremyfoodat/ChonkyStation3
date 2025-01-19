#pragma once

#include <common.hpp>
#include <logger.hpp>
#include <BEField.hpp>

#include <CellTypes.hpp>


// Circular dependency
class PlayStation3;

using namespace CellTypes;

class CellSpurs {
public:
    CellSpurs(PlayStation3* ps3) : ps3(ps3) {}
    PlayStation3* ps3;

    u64 cellSpursCreateTaskset();
    u64 _cellSpursEventFlagInitialize();
    u64 cellSpursEventFlagAttachLv2EventQueue();
    u64 cellSpursInitialize();
    u64 cellSpursAttachLv2EventQueue();
    u64 cellSpursCreateTask();

private:
    MAKE_LOG_FUNCTION(log, cellSpurs);
};