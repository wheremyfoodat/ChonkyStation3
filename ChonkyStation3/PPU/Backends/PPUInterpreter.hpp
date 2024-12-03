#pragma once

#include <PPU.hpp>


using namespace PPUTypes;

class PPUInterpreter : public PPU {
public:
	PPUInterpreter(Memory& mem) : PPU(mem) {}
	void step() override;

	void addi(const Instruction& instr);
};