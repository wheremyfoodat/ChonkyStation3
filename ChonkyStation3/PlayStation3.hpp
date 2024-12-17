#pragma once

#include <common.hpp>
#include <Memory.hpp>
#include <ELF/ELFloader.hpp>
#include <PPU.hpp>
#include <ModuleManager.hpp>
#include <ThreadManager.hpp>
#include <HandleManager.hpp>
#include <Syscall.hpp>
#include <Backends/PPUInterpreter.hpp>
#include <unordered_map>


class PlayStation3 {
public:
    PlayStation3(const fs::path& executable);
    Memory mem = Memory();
    PPU* ppu;
    PPUInterpreter interpreter;
    ModuleManager module_manager;
    ThreadManager thread_manager;
    HandleManager handle_manager;
    Syscall syscall;

    void step();
};
