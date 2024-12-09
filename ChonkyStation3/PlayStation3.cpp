#include "PlayStation3.hpp"


PlayStation3::PlayStation3(const fs::path& executable) : mem(), interpreter(mem, this), syscall(this), module_manager(this), thread_manager(this) {
    ppu = &interpreter;
    ELFLoader elf = ELFLoader(mem);
    std::unordered_map<u32, u32> imports = {};
    // Load ELF file
    auto entry = elf.load(executable, imports);
    // Register ELF module imports in module manager
    for (auto& i : imports)
        module_manager.registerImport(i.first, i.second);
    // Create start thread
    u8 thread_name[] = "___start___";
    thread_manager.createThread(entry, DEFAULT_STACK_SIZE, 0, thread_name, true);
}

void PlayStation3::step() {
    ppu->step();
}