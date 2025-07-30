#pragma once

#include <common.hpp>
#include <BitField.hpp>

#include <Memory.hpp>
#include <PPUTypes.hpp>
#include <PPUDisassembler.hpp>
#include <cstdio>

// Circular dependency
class PlayStation3;

class PPU {
protected:
    PPUTypes::State stateInternal;

public:
    // We have two constructor overloads.
    // The first one uses an internal state struct for registers, and is used in the PPU interpreter core
    PPU(Memory& mem, PlayStation3* ps3) : mem(mem), ps3(ps3), state(stateInternal) {}
    // The second one uses an external state struct for registers, and is used for interpreter fallbacks in the JIT cores
    PPU(Memory& mem, PlayStation3* ps3, PPUTypes::State& state) : mem(mem), ps3(ps3), state(state) {}

    Memory& mem;
    PlayStation3* ps3;
    virtual int step(); // Returns number of cycles executed
    virtual void printCallStack() { printf("Callstack not available for PPU core\n"); }

    void runFunc(u32 addr, u32 toc = 0, bool save_all_state = true);

    PPUTypes::State& state;

    bool should_log = false;    // For debugging, unused normally

    void printState();
    bool doesAnyRegContain(u64 val);
    bool doesAnyRegContainMasked(u64 val, u64 mask);

    bool branchCondition(u8 bo, u8 bi);
};
