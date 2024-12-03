#include "PPUInterpreter.hpp"


void PPUInterpreter::step() {
	const u32 instrRaw = mem.read<u32>(state.pc);
	const Instruction instr { .raw = instrRaw };
	
	PPUDisassembler::disasm(state, instr);
	
	switch (instr.opc) {
	
	case ADDI: addi(instr); break;

	default:
		Helpers::panic("Unimplemented opcode 0x%02x (decimal: %d) (full instr: 0x%08x)\n", (u32)instr.opc, (u32)instr.opc, instr.raw);
	}

	state.pc += 4;
}

void PPUInterpreter::addi(const Instruction& instr) {
	state.gprs[instr.rt] = state.gprs[instr.ra] + (s64)(s16)instr.si;
}