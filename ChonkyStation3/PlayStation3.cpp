#include "PlayStation3.hpp"


PlayStation3::PlayStation3(const fs::path& executable) : mem(), interpreter(mem, this), syscall(this), module_manager(this), thread_manager(this) {
    ppu = &interpreter;
    ELFLoader elf = ELFLoader(mem);
    std::unordered_map<u32, u32> imports = {};
    // Load ELF file
    auto entry = elf.load(executable, imports, module_manager);
    // Register ELF module imports in module manager
    for (auto& i : imports)
        module_manager.registerImport(i.first, i.second);
    // Create main thread
    u8 thread_name[] = "main";
    Thread* main_thread = thread_manager.createThread(entry, DEFAULT_STACK_SIZE, 0, thread_name, elf.tls_vaddr, elf.tls_filesize, elf.tls_memsize, true);
}

void PlayStation3::step() {
    ppu->step();
}