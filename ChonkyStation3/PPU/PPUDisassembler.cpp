#include "PPUDisassembler.hpp"


void PPUDisassembler::disasm(PPUTypes::State& state, PPUTypes::Instruction instr) {
	switch (instr.opc) {
	
	case PPUTypes::Instructions::ADDI: printf("0x%016llx | addi r%d, r%d, 0x%04x\n", state.pc, (u8)instr.rt, (u8)instr.ra, (u8)instr.si); break;

	default: printf("0x%016llx | unknown 0x%02x\n", state.pc, (u8)instr.opc); break;
	}
}