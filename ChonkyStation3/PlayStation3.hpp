#pragma once

#include <common.hpp>

#include <functional>
#include <unordered_map>

#include <PPU.hpp>
#include <Backends/PPUInterpreter.hpp>
#include <RSX.hpp>
#include <Memory.hpp>
#include <ELF/ELFLoader.hpp>
#include <SFO/SFOLoader.hpp>
#include <Game/GameLoader.hpp>
#include <ElfSymbolParser.hpp>
#include <ModuleManager.hpp>
#include <ThreadManager.hpp>
#include <HandleManager.hpp>
#include <PRXManager.hpp>
#include <Lv2ObjectManager.hpp>
#include <Syscall.hpp>
#include <Scheduler.hpp>
#include <Filesystem.hpp>
#include <CrashAnalyzer.hpp>


class PlayStation3 {
public:
    PlayStation3(const fs::path& executable = "");
    ~PlayStation3();
    Memory mem = Memory();
    PPU* ppu;
    RSX rsx;
    PPUInterpreter interpreter;
    ModuleManager module_manager;
    ThreadManager thread_manager;
    HandleManager handle_manager;
    PRXManager prx_manager;
    Lv2ObjectManager lv2_obj;
    Syscall syscall;
    ElfSymbolParser elf_parser;
    Scheduler scheduler;
    Filesystem fs;
    
    CrashAnalyzer crash_analyzer;
    GameLoader::InstalledGame curr_game;
    fs::path elf_path;
    std::string elf_path_encrypted;

    std::function<void(void)> flip_handler;
    u64 cycle_count = 0;
    u64 curr_block_cycles = 0;
    u64 skipped_cycles = 0;
    bool force_scheduler_update = false;

    void gameSelector();
    void loadGame(const GameLoader::InstalledGame& game);
    void setFlipHandler(std::function<void(void)> const& handler);
    void init();
    void run();
    void step();
    void flip();
    void vblank();
    void skipToNextEvent();
    void forceSchedulerUpdate();

    void pressButton(u32 button);
    void resetButtons();
};
