#include "PlayStation3.hpp"


PlayStation3::PlayStation3(const fs::path& executable) : mem(), interpreter(mem) {
	mem = Memory();
	ppu = &interpreter;
	ELFLoader elf = ELFLoader(mem);
	ppu->state.pc = elf.load(executable);
}

void PlayStation3::step() {
	ppu->step();
}