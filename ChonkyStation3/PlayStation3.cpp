#include "PlayStation3.hpp"


PlayStation3::PlayStation3(const fs::path& executable) : mem(), interpreter(mem) {
	mem = Memory();
	ppu = &interpreter;
	ELFLoader elf = ELFLoader(mem);
	ppu->state.pc = elf.load(executable);
	// Setup stack
	// TODO: will have to slightly change this to allow multiple threads in the future
	u64 stack = mem.alloc(DEFAULT_STACK_SIZE);
	mem.mmap(STACK_REGION_START, stack, DEFAULT_STACK_SIZE);
	ppu->state.gprs[1] = STACK_REGION_START + DEFAULT_STACK_SIZE;	// Stack pointer
}

void PlayStation3::step() {
	ppu->step();
}