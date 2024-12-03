#pragma once

#include <common.hpp>
#include <PPUTypes.hpp>


class PPUDisassembler {
public:
	static void disasm(PPUTypes::State& state, PPUTypes::Instruction instr);
};