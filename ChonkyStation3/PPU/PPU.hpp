#pragma once

#include <common.hpp>
#include <Memory.hpp>
#include <BitField.hpp>
#include <PPUTypes.hpp>
#include <PPUDisassembler.hpp>


class PPU {
public:
	PPU(Memory& mem) : mem(mem) {}
	Memory& mem;
	virtual void step();

	PPUTypes::State state;
};