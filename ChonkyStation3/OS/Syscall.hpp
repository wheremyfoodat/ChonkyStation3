#pragma once

#include <common.hpp>
#include <CellTypes.hpp>


// Circular dependency
class PlayStation3;

using namespace CellTypes;

class Syscall {
public:
    Syscall(PlayStation3* ps3);
    PlayStation3* ps3;

    void doSyscall(bool decrement_pc_if_module_call = false);

    // sysMMapper
    u64 sysMMapperAllocateAddress();

    // sysMemory
    u64 sysMemoryGetUserMemorySize();
};