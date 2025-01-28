#include "PlayStation3.hpp"


PlayStation3::PlayStation3(const fs::path& executable) : elf_parser(executable), interpreter(mem, this), rsx(this), syscall(this), module_manager(this), thread_manager(this), prx_manager(this), fs(this) {
    ppu = &interpreter;
    
    // Initialize filesystem
    fs.mount(Filesystem::Device::DEV_FLASH, "./Filesystem/dev_flash/");
    fs.mount(Filesystem::Device::DEV_HDD0, "./Filesystem/dev_hdd0/");
    
    // Load ELF file
    ELFLoader elf = ELFLoader(this, mem);
    std::unordered_map<u32, u32> imports = {};
    auto entry = elf.load(executable, imports, module_manager);
    
    // Register ELF module imports in module manager
    for (auto& i : imports)
        module_manager.registerImport(i.first, i.second);

    // Create main thread
    thread_manager.setTLS(elf.tls_vaddr, elf.tls_filesize, elf.tls_memsize);
    Thread* main_thread = thread_manager.createThread(entry, DEFAULT_STACK_SIZE, 0, (const u8*)"main", elf.tls_vaddr, elf.tls_filesize, elf.tls_memsize, true);

    // Load PRXs required by the ELF
    prx_manager.loadModulesRecursively();
    
    // Initialize libraries (must be done after creating main thread)
    prx_manager.initializeLibraries();
}

void PlayStation3::run() {
    skipped_cycles = 0;
    while (cycle_count++ < CPU_FREQ) {
        step();
    }
    cycle_count = 0;
}

void PlayStation3::step() {
    ppu->step();
    scheduler.tick(1);
}

void PlayStation3::skipToNextEvent() {
    const u64 ticks = scheduler.tickToNextEvent();
    cycle_count += ticks;
    skipped_cycles += ticks;
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