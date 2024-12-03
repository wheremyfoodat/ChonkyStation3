#pragma once

#include <BitField.hpp>


namespace PPUTypes {

struct State {
	u64 gprs[32] = { 0 };
	u64 pc = 0;
};

union Instruction {
	u32 raw;
	BitField<0, 16, u32> si;
	BitField<16, 5, u32> ra;
	BitField<21, 5, u32> rt;
	BitField<26, 6, u32> opc;
};

enum Instructions {
	ADDI = 0x0E,
};

} // End namespace PPUTypes