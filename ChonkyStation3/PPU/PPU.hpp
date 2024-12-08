#pragma once

#include <common.hpp>
#include <Memory.hpp>
#include <BitField.hpp>
#include <PPUTypes.hpp>
#include <PPUDisassembler.hpp>

// Circular dependency
class PlayStation3;

class PPU {
public:
    PPU(Memory& mem, PlayStation3* ps3) : mem(mem), ps3(ps3) {}
    Memory& mem;
    PlayStation3* ps3;
    virtual void step();

    PPUTypes::State state;

    bool branchCondition(u8 bo, u8 bi);
};