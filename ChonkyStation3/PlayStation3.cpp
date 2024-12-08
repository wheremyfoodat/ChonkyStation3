#include "PlayStation3.hpp"


PlayStation3::PlayStation3(const fs::path& executable) : mem(), interpreter(mem, this), syscall(this) {
    mem = Memory();
    module_manager = ModuleManager();
    ppu = &interpreter;
    ELFLoader elf = ELFLoader(mem);
    std::unordered_map<u32, u32> imports = {};
    ppu->state.pc = elf.load(executable, imports);
    for (auto& i : imports)
        module_manager.registerImport(i.first, i.second);
    // Setup stack
    // TODO: will have to slightly change this to allow multiple threads in the future
    u64 stack = mem.alloc(DEFAULT_STACK_SIZE);
    mem.mmap(STACK_REGION_START, stack, DEFAULT_STACK_SIZE);
    ppu->state.gprs[1] = STACK_REGION_START + DEFAULT_STACK_SIZE;   // Stack pointer
}

void PlayStation3::step() {
    ppu->step();
}