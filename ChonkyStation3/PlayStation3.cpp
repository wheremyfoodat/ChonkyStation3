#include "PlayStation3.hpp"


PlayStation3::PlayStation3(const fs::path& executable) : elf_parser(executable), interpreter(mem, this), spu_interpreter(this), rsx(this), syscall(this), module_manager(this), thread_manager(this), spu_thread_manager(this), prx_manager(this), fs(this), lv2_obj(this, &handle_manager) {
    ppu = &interpreter;
    spu = &spu_interpreter;
    
    // Load settings
    settings.load();

    module_manager.init();

    // Initialize filesystem
    fs.mount(Filesystem::Device::DEV_HDD0, settings.filesystem.dev_hdd0_mountpoint);
    fs.mount(Filesystem::Device::DEV_HDD1, settings.filesystem.dev_hdd1_mountpoint);
    fs.mount(Filesystem::Device::DEV_FLASH, settings.filesystem.dev_flash_mountpoint);
    fs.mount(Filesystem::Device::DEV_USB000, settings.filesystem.dev_usb000_mountpoint);
    fs.initialize();

    // An executable was passed as CLI argument
    if (!(executable.generic_string() == "")) {
        elf_path = executable;
    }
}

PlayStation3::~PlayStation3() {
    // Free memory
    for (auto& i : mem.regions)
        free(i->mem);
}

void PlayStation3::gameSelector() {
    // Find installed games
    GameLoader game_loader = GameLoader(this);  // Requires dev_hdd0 to be mounted
    // Print games
    printf("Found %lld installed games:\n", game_loader.games.size());
    for (int i = 0; i < game_loader.games.size(); i++) {
        printf("%s\n", std::format("[{:>3d}] {} | {:<40s} | {:<40s}", i, game_loader.games[i].id, game_loader.games[i].title, game_loader.games[i].content_path.generic_string()).c_str());
    }
    // Ask the user what game to run
    int idx;
    printf("Enter game to run (index): ");
    std::cin >> idx;
    while (idx < 0 || idx >= game_loader.games.size()) {
        printf("Invalid index. Retry: ");
        std::cin >> idx;
    }

    loadGame(game_loader.games[idx]);
}

void PlayStation3::loadGame(const GameLoader::InstalledGame& game) {
    curr_game = game;
    // Tell cellGame the path of the game's contents
    module_manager.cellGame.setContentPath(game.content_path);
    // Get path of EBOOT.elf
    elf_path = fs.guestPathToHost(game.content_path / "USRDIR/EBOOT.elf");
    elf_path_encrypted = (game.content_path / "USRDIR/EBOOT.BIN").generic_string();
}

void PlayStation3::setFlipHandler(std::function<void(void)> const& handler) {
    flip_handler = handler;
}

void PlayStation3::init() {
    // Only init if we aren't replaying an RSX capture (aka if we actually booted something)
    if (!rsx_capture_path.empty()) return;

    // Load ELF file
    ELFLoader elf = ELFLoader(this, mem);
    std::unordered_map<u32, u32> imports = {};
    ELFLoader::PROCParam proc_param;
    auto entry = elf.load(elf_path, imports, proc_param, module_manager);

    // Mount /app_home
    fs.mount(Filesystem::Device::APP_HOME, elf_path.parent_path());

    // Register ELF module imports in module manager
    for (auto& i : imports)
        module_manager.registerImport(i.first, i.second);

    // Create main thread
    thread_manager.setTLS(elf.tls_vaddr, elf.tls_filesize, elf.tls_memsize);
    elf_path_encrypted += '\0';
    // TODO: Should the main thread have priority 0?
    Thread* main_thread = thread_manager.createThread(entry, DEFAULT_STACK_SIZE, 0, 0, (const u8*)"main", elf.tls_vaddr, elf.tls_filesize, elf.tls_memsize, true, elf_path_encrypted);
    ppu->state.gprs[11] = entry;
    ppu->state.gprs[12] = proc_param.malloc_pagesize ? proc_param.malloc_pagesize : 0x100000;

    prx_manager.createLv2PRXs();

    // Load PRXs required by the ELF
    prx_manager.loadModulesRecursively();

    // Initialize libraries (must be done after creating main thread)
    try {
        prx_manager.initializeLibraries();
    }
    catch (std::runtime_error e) {
        printCrashInfo(e);
    }

    // SPU Debugging options
    if (!settings.debug.enable_spu_after_pc.empty()) {
        enable_spu_on_pc = std::stoul(settings.debug.enable_spu_after_pc, 0, 16);
        spu_thread_to_enable = settings.debug.spu_thread_to_enable;
        mem.watchpoints_r[enable_spu_on_pc] = std::bind(&PlayStation3::enableSPUOnPC, this, std::placeholders::_1);
        mem.markAsSlowMem(enable_spu_on_pc >> PAGE_SHIFT, true, false);
        printf("Will enable SPU Thread %s on pc 0x%08x\n", spu_thread_to_enable.c_str(), enable_spu_on_pc);
    }
}

void PlayStation3::run() {
    try {
        curr_block_cycles = 0;
        curr_block = 0;
        while (cycle_count < CPU_FREQ)
            step();
        cycle_count = 0;
    }
    catch (std::runtime_error e) {
        printCrashInfo(e);
    }
}

static constexpr int reschedule_every_n_blocks = 48;
void PlayStation3::step() {
    ppu->step();
    spu->step();

    if (force_scheduler_update || curr_block_cycles++ >= 2048) {
        scheduler.tick(curr_block_cycles);
        cycle_count += curr_block_cycles;
        
        curr_block_cycles = 0;
        force_scheduler_update = false;

        if (curr_block++ >= reschedule_every_n_blocks) {
            curr_block = 0;
            thread_manager.reschedule();
        }
    }
}

void PlayStation3::printCrashInfo(std::runtime_error err) {
    printf("PPU state:\n");
    ppu->printState();
    printf("SPU state:\n");
    spu->printState();

    const std::string error = err.what();
    printf("FATAL: %s\n", err.what());

#ifndef CHONKYSTATION3_USER_BUILD
    ((PPUInterpreter*)ppu)->printCallStack();

    //printf("The crash happened at the following instruction:\n");
    //PPUDisassembler::disasm(ppu->state, crash_analyzer.lastInstr(), &mem);
    //crash_analyzer.analyzeCrash(error);
#endif
    std::exit(0);
}

void PlayStation3::flip() {
    module_manager.cellGcmSys.flip = 0;
    if (module_manager.cellGcmSys.flip_callback) {
        u32 old_r3 = ppu->state.gprs[3];
        ppu->state.gprs[3] = 1; // Callback function is always called with 1 as first argument
        ppu->runFunc(mem.read<u32>(module_manager.cellGcmSys.flip_callback), mem.read<u32>(module_manager.cellGcmSys.flip_callback + 4));
        ppu->state.gprs[3] = old_r3;
    }
    mem.write<u64>(module_manager.cellGcmSys.label_addr + 0x10, 0);
    mem.write<u64>(module_manager.cellGcmSys.label_addr + 0x10 + 8, 0);

    flip_handler();
}

void PlayStation3::vblank() {
    if (module_manager.cellGcmSys.vblank_handler) {
        u32 old_r3 = ppu->state.gprs[3];
        ppu->state.gprs[3] = 1; // Handler function is always called with 1 as first argument
        ppu->runFunc(mem.read<u32>(module_manager.cellGcmSys.vblank_handler), mem.read<u32>(module_manager.cellGcmSys.vblank_handler + 4));
        ppu->state.gprs[3] = old_r3;
    }
}

void PlayStation3::skipToNextEvent() {
    const u64 ticks = scheduler.tickToNextEvent();
    cycle_count += ticks;
    skipped_cycles += ticks;
}

void PlayStation3::forceSchedulerUpdate() {
    force_scheduler_update = true;
}

void PlayStation3::pressButton(u32 button) {
    int idx = 2;
    if (button >= (1 << 16)) {
        idx = 3;
        button >>= 16;
    }
    module_manager.cellPad.buttons[idx] |= button;
}

void PlayStation3::setLeftStick(float x, float y) {
    module_manager.cellPad.buttons[6] = (u8)(x * 255);
    module_manager.cellPad.buttons[7] = (u8)(y * 255);
}

void PlayStation3::setRightStick(float x, float y) {
    module_manager.cellPad.buttons[4] = (u8)(x * 255);
    module_manager.cellPad.buttons[5] = (u8)(y * 255);
}

void PlayStation3::resetButtons() {
    for (int i = 0; i < CELL_PAD_MAX_CODES; i++)
        module_manager.cellPad.buttons[i] = 0;
}

void PlayStation3::enableSPUOnPC(u32 unused) {
    if (ppu->state.pc == enable_spu_on_pc) {
        for (auto& i : spu_thread_manager.threads) {
            if (i.name == spu_thread_to_enable) {
                i.wakeUp();
                mem.markAsFastMem(enable_spu_on_pc >> PAGE_SHIFT, mem.getPtr(enable_spu_on_pc & ~PAGE_MASK), true, true);
                return;
            }
        }
    }
}