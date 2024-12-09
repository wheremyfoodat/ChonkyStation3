#pragma once

#include <common.hpp>
#include <Memory.hpp>
#include <ELF/ELFloader.hpp>
#include <PPU.hpp>
#include <ModuleManager.hpp>
#include <ThreadManager.hpp>
#include <Syscall.hpp>
#include <Backends/PPUInterpreter.hpp>
#include <unordered_map>


class PlayStation3 {
public:
    PlayStation3(const fs::path& executable);
    Memory mem;
    PPU* ppu;
    PPUInterpreter interpreter;
    ModuleManager module_manager;
    ThreadManager thread_manager;
    Syscall syscall;

    void step();
};
