#include "PlayStation3.hpp"


PlayStation3::PlayStation3(const fs::path& executable) : elf_parser(executable), interpreter(mem, this), rsx(this), syscall(this), module_manager(this), thread_manager(this), prx_manager(this), fs(this), lv2_obj(this, &handle_manager) {
    ppu = &interpreter;
    
    module_manager.init();

    // Initialize filesystem
    fs.mount(Filesystem::Device::DEV_FLASH, "./Filesystem/dev_flash/");
    fs.mount(Filesystem::Device::DEV_HDD0, "./Filesystem/dev_hdd0/");
    fs.mount(Filesystem::Device::DEV_HDD1, "./Filesystem/dev_hdd1/");
    fs.mount(Filesystem::Device::DEV_USB000, "./Filesystem/dev_usb000/");
    fs.mount(Filesystem::Device::APP_HOME, "./Filesystem/app_home/");
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

void PlayStation3::init() {
    // Load ELF file
    ELFLoader elf = ELFLoader(this, mem);
    std::unordered_map<u32, u32> imports = {};
    ELFLoader::PROCParam proc_param;
    auto entry = elf.load(elf_path, imports, proc_param, module_manager);

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

    // Load PRXs required by the ELF
    prx_manager.loadModulesRecursively();

    // Initialize libraries (must be done after creating main thread)
    prx_manager.initializeLibraries();
}

void PlayStation3::run() {
    try {
        skipped_cycles = 0;
        static constexpr int reschedule_every_n_blocks = 256;
        int curr_block = 0;

        while (cycle_count < CPU_FREQ) {
            while (curr_block_cycles++ < 2048) {
                step();
                if (force_scheduler_update) {
                    force_scheduler_update = false;
                    break;
                }
            }
            cycle_count += curr_block_cycles;
            scheduler.tick(curr_block_cycles);
            curr_block_cycles = 0;
            curr_block++;
            if (curr_block >= reschedule_every_n_blocks) {
                curr_block = 0;
                thread_manager.reschedule();
            }
        }
        cycle_count = 0;
    }
    catch (std::exception e) {
        ppu->printState();

        const std::string error = e.what();
        printf("FATAL: %s\n", e.what());
        
#ifndef CHONKYSTATION3_USER_BUILD
        ((PPUInterpreter*)ppu)->printCallStack();

        //printf("The crash happened at the following instruction:\n");
        //PPUDisassembler::disasm(ppu->state, crash_analyzer.lastInstr(), &mem);
        //crash_analyzer.analyzeCrash(error);
#endif
        std::exit(0);
    }
}

void PlayStation3::step() {
    ppu->step();
}

void PlayStation3::flip() {
    module_manager.cellGcmSys.flip = 0;
    if (module_manager.cellGcmSys.flip_callback) {
        u32 old_r3 = ppu->state.gprs[3];
        ppu->state.gprs[3] = 1; // Callback function is always called with 1 as first argument
        //ppu->runFunc(mem.read<u32>(module_manager.cellGcmSys.flip_callback), mem.read<u32>(module_manager.cellGcmSys.flip_callback + 4));
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

void PlayStation3::resetButtons() {
    for (int i = 0; i < CELL_PAD_MAX_CODES; i++)
        module_manager.cellPad.buttons[i] = 0;
}