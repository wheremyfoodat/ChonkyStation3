#pragma once

#include <PPU.hpp>

// Circular dependency
class PlayStation3;

using namespace PPUTypes;

class PPUInterpreter : public PPU {
public:
    PPUInterpreter(Memory& mem, PlayStation3* ps3) : PPU(mem, ps3) {}
    void step() override;

    // Main
    void cmpi   (const Instruction& instr);
    void addi   (const Instruction& instr);
    void bc     (const Instruction& instr);
    void sc     (const Instruction& instr);
    void b      (const Instruction& instr);
    void ori    (const Instruction& instr);
    void oris   (const Instruction& instr);
    void xori   (const Instruction& instr);
    void xoris  (const Instruction& instr);
    void andi   (const Instruction& instr);
    void andis  (const Instruction& instr);
    void lwz    (const Instruction& instr);
    void stw    (const Instruction& instr);
    // G_13
    void bcctr  (const Instruction& instr);
    // G_1E
    void rldicl (const Instruction& instr);
    // G_1F
    void cmpl   (const Instruction& instr);
    void mfspr  (const Instruction& instr);
    void or_    (const Instruction& instr);
    void mtspr  (const Instruction& instr);
    // G_3A
    void ld(const Instruction& instr);
    void ldu(const Instruction& instr);
    // G_3E
    void std    (const Instruction& instr);
    void stdu   (const Instruction& instr);
};