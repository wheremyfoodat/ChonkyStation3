#include "PPUInterpreter.hpp"


void PPUInterpreter::step() {
    const u32 instrRaw = mem.read<u32>(state.pc);
    const Instruction instr { .raw = instrRaw };
    
    PPUDisassembler::disasm(state, instr);
    
    switch (instr.opc) {
    
    case ADDI:  addi(instr);    break;
    case ORI:   ori(instr);     break;
    case ORIS:  oris(instr);    break;
    case XORI:  xori(instr);    break;
    case XORIS: xoris(instr);   break;
    case ANDI:  andi(instr);    break;
    case ANDIS: andis(instr);   break;
    case LWZ:   lwz(instr);     break;
    case G_3E: {
        switch (instr.g_3e_field) {

        case STD:   std(instr);     break;
        case STDU:  stdu(instr);    break;

        default:
            Helpers::panic("Unimplemented G_3E instruction 0x%02x (decimal: %d) (full instr: 0x%08x)\n", (u32)instr.g_3e_field, (u32)instr.g_3e_field, instr.raw);
        }
        break;
    }

    default:
        Helpers::panic("Unimplemented opcode 0x%02x (decimal: %d) (full instr: 0x%08x)\n", (u32)instr.opc, (u32)instr.opc, instr.raw);
    }

    state.pc += 4;
}

// Main

void PPUInterpreter::addi(const Instruction& instr) {
    state.gprs[instr.rt] = state.gprs[instr.ra] + (s64)(s16)instr.si;
}

void PPUInterpreter::ori(const Instruction& instr) {
    state.gprs[instr.ra] = state.gprs[instr.rs] | instr.ui;
}

void PPUInterpreter::oris(const Instruction& instr) {
    state.gprs[instr.ra] = state.gprs[instr.rs] | (instr.ui << 16u);
}

void PPUInterpreter::xori(const Instruction& instr) {
    state.gprs[instr.ra] = state.gprs[instr.rs] ^ instr.ui;
}

void PPUInterpreter::xoris(const Instruction& instr) {
    state.gprs[instr.ra] = state.gprs[instr.rs] ^ (instr.ui << 16u);
}

void PPUInterpreter::andi(const Instruction& instr) {
    state.gprs[instr.ra] = state.gprs[instr.rs] & instr.ui;
}

void PPUInterpreter::andis(const Instruction& instr) {
    state.gprs[instr.ra] = state.gprs[instr.rs] & (instr.ui << 16u);
}

void PPUInterpreter::lwz(const Instruction& instr) {
    const s32 sd = (s32)(s16)instr.d;
    const u32 addr = (instr.ra == 0) ? sd : state.gprs[instr.ra] + sd;
    state.gprs[instr.rt] = mem.read<u32>(addr);
}

// G_3E

void PPUInterpreter::std(const Instruction& instr) {
    const s32 sds = (s32)(s16)(instr.ds << 2);
    const u32 addr = state.gprs[instr.ra] + sds;
    mem.write(addr, state.gprs[instr.rs]);
}

void PPUInterpreter::stdu(const Instruction& instr) {
    const s32 sds = (s32)(s16)(instr.ds << 2);
    const u32 addr = state.gprs[instr.ra] + sds;
    mem.write(addr, state.gprs[instr.rs]);
    state.gprs[instr.ra] = addr;    // Update
}