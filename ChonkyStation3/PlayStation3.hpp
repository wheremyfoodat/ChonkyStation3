#pragma once

#include <common.hpp>

#include <unordered_map>

#include <PPU.hpp>
#include <Backends/PPUInterpreter.hpp>
#include <RSX.hpp>
#include <Memory.hpp>
#include <ELF/ELFLoader.hpp>
#include <ElfSymbolParser.hpp>
#include <ModuleManager.hpp>
#include <ThreadManager.hpp>
#include <HandleManager.hpp>
#include <PRXManager.hpp>
#include <Syscall.hpp>
#include <Scheduler.hpp>


class PlayStation3 {
public:
    PlayStation3(const fs::path& executable);
    Memory mem = Memory();
    PPU* ppu;
    RSX rsx;
    PPUInterpreter interpreter;
    ModuleManager module_manager;
    ThreadManager thread_manager;
    HandleManager handle_manager;
    PRXManager prx_manager;
    Syscall syscall;
    ElfSymbolParser elf_parser;
    Scheduler scheduler;

    u64 cycle_count = 0;
    void run();
    void step();
    void skipToNextEvent();
};
