#include "PlayStation3.hpp"


PlayStation3::PlayStation3(const fs::path& executable) : elf_parser(executable), interpreter(mem, this), rsx(this), syscall(this), module_manager(this), thread_manager(this), prx_manager(this) {
    ppu = &interpreter;
    ELFLoader elf = ELFLoader(this, mem);
    std::unordered_map<u32, u32> imports = {};

    // Load ELF file
    auto entry = elf.load(executable, imports, module_manager);
    
    // Register ELF module imports in module manager
    for (auto& i : imports)
        module_manager.registerImport(i.first, i.second);
    

    // Create main thread
    u8 thread_name[] = "main";
    Thread* main_thread = thread_manager.createThread(entry, DEFAULT_STACK_SIZE, 0, thread_name, elf.tls_vaddr, elf.tls_filesize, elf.tls_memsize, true);

    // Load PRXs required by the ELF
    prx_manager.loadModulesRecursively();
    
    // Initialize libraries (must be done after creating main thread)
    prx_manager.initializeLibraries();

    //printf("Found debug symbols:\n");
    //for (auto& i : elf_parser.symbolMap)
    //    printf("%s @ 0x%08x\n", i.second.name.c_str(), i.first);
}

void PlayStation3::run() {
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
}