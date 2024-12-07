#pragma once

#include <common.hpp>
#include <Memory.hpp>
#include <ELF/ELFloader.hpp>
#include <PPU.hpp>
#include <Backends/PPUInterpreter.hpp>


class PlayStation3 {
public:
    PlayStation3(const fs::path& executable);
    Memory mem;
    PPU* ppu;
    PPUInterpreter interpreter;

    void step();
};
