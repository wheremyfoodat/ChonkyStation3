#pragma once

#include <common.hpp>
#include <logger.hpp>

#include <CellTypes.hpp>


// Circular dependency
class PlayStation3;

using namespace CellTypes;

class Syscall {
public:
    Syscall(PlayStation3* ps3);
    PlayStation3* ps3;
    MAKE_LOG_FUNCTION(log, misc_sc);
    MAKE_LOG_FUNCTION(unimpl, unimplemented);
    MAKE_LOG_FUNCTION(tty, tty);

    void doSyscall(bool decrement_pc_if_module_call = false);

    // sysMMapper
    u64 sysMMapperAllocateAddress();
    u64 sysMMapperSearchAndMapMemory();

    // sysMemory
    u64 sysMemoryAllocate();
    u64 sysMemoryGetUserMemorySize();

    // sysTimer
    u64 sysTimerUsleep();
};