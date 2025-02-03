#include "PlayStation3.hpp"


PlayStation3::PlayStation3(const fs::path& executable) : elf_parser(executable), interpreter(mem, this), rsx(this), syscall(this), module_manager(this), thread_manager(this), prx_manager(this), fs(this), lv2_obj(this, &handle_manager) {
    ppu = &interpreter;
    
    // Initialize filesystem
    fs.mount(Filesystem::Device::DEV_FLASH, "./Filesystem/dev_flash/");
    fs.mount(Filesystem::Device::DEV_HDD0, "./Filesystem/dev_hdd0/");
    fs.mount(Filesystem::Device::APP_HOME, "./Filesystem/app_home/");

    fs::path elf_path;
    std::string elf_path_encrypted;
    // An executable was passed as CLI argument, run it directly
    if (!(executable.generic_string() == "")) {
        elf_path = executable;
    }
    else {
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

        curr_game = game_loader.games[idx];
        // Tell cellGame the path of the game's contents
        module_manager.cellGame.setContentPath(game_loader.games[idx].content_path);
        // Get path of EBOOT.elf
        elf_path = fs.guestPathToHost(game_loader.games[idx].content_path / "USRDIR/EBOOT.elf");
        elf_path_encrypted = (game_loader.games[idx].content_path / "USRDIR/EBOOT.BIN").generic_string();
    }
    
    // Load ELF file
    ELFLoader elf = ELFLoader(this, mem);
    std::unordered_map<u32, u32> imports = {};
    auto entry = elf.load(elf_path, imports, module_manager);
    
    // Register ELF module imports in module manager
    for (auto& i : imports)
        module_manager.registerImport(i.first, i.second);

    // Create main thread
    thread_manager.setTLS(elf.tls_vaddr, elf.tls_filesize, elf.tls_memsize);
    elf_path_encrypted += '\0';
    Thread* main_thread = thread_manager.createThread(entry, DEFAULT_STACK_SIZE, 0, (const u8*)"main", elf.tls_vaddr, elf.tls_filesize, elf.tls_memsize, true, elf_path_encrypted);

    // Load PRXs required by the ELF
    prx_manager.loadModulesRecursively();
    
    // Initialize libraries (must be done after creating main thread)
    prx_manager.initializeLibraries();
}

void PlayStation3::run() {
    skipped_cycles = 0;
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
    }
    cycle_count = 0;
}

void PlayStation3::step() {
    ppu->step();
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