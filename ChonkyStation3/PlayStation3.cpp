#include "PlayStation3.hpp"


PlayStation3::PlayStation3(const fs::path& executable) {
	mem = Memory();
	ELFLoader elf = ELFLoader(mem);
	elf.load(executable);
}