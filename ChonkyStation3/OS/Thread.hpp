#pragma once

#include <common.hpp>
#include <PPUTypes.hpp>
#include <MemoryConstants.hpp>


using namespace PPUTypes;

// Circular dependency
class ThreadManager;

class Thread {
public:
    Thread(u64 entry, u64 stack_size, u64 arg, u8* name, int id, ThreadManager* mgr);
    ThreadManager* mgr;
    State state;
    u64 stack;
    u64 stack_size;
    std::string name;
    int id;
};