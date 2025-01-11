#pragma once

#include <common.hpp>
#include <Memory.hpp>
#include <ELF/ELFloader.hpp>
#include <ElfSymbolParser.hpp>
#include <PPU.hpp>
#include <RSX.hpp>
#include <ModuleManager.hpp>
#include <ThreadManager.hpp>
#include <HandleManager.hpp>
#include <Syscall.hpp>
#include <Scheduler.hpp>
#include <Backends/PPUInterpreter.hpp>
#include <unordered_map>


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
    Syscall syscall;
    ElfSymbolParser elf_parser;
    Scheduler scheduler;

    u64 cycle_count = 0;
    void run();
    void step();
    void skipToNextEvent();
};
