#pragma once

#include <PPU.hpp>


using namespace PPUTypes;

class PPUInterpreter : public PPU {
public:
	PPUInterpreter(Memory& mem) : PPU(mem) {}
	void step() override;

	// Main
	void addi	(const Instruction& instr);
	void ori	(const Instruction& instr);
	void oris	(const Instruction& instr);
	void xori	(const Instruction& instr);
	void xoris	(const Instruction& instr);
	void andi	(const Instruction& instr);
	void andis	(const Instruction& instr);
	void lwz	(const Instruction& instr);
	// G_3E
	void std	(const Instruction& instr);
	void stdu	(const Instruction& instr);
};