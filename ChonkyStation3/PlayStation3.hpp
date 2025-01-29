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
#include <Filesystem.hpp>


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
    Filesystem fs;

    u64 cycle_count = 0;
    u64 curr_block_cycles = 0;
    u64 skipped_cycles = 0;
    bool force_scheduler_update = false;

    void run();
    void step();
    void skipToNextEvent();
    void forceSchedulerUpdate();

    void pressButton(u32 button);
    void resetButtons();
};
