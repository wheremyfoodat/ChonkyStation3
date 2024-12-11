#include "PPUInterpreter.hpp"
#include <PlayStation3.hpp>


void PPUInterpreter::step() {
    const u32 instrRaw = mem.read<u32>(state.pc);
    const Instruction instr { .raw = instrRaw };
    
    PPUDisassembler::disasm(state, instr, &mem);

    switch (instr.opc) {
    
    case CMPI:  cmpi(instr);    break;
    case ADDI:  addi(instr);    break;
    case BC:    bc(instr);      break;
    case SC:    sc(instr);      break;
    case B:     b(instr);       break;
    case G_13: {
        switch (instr.g_13_field) {

        case BCCTR: bcctr(instr); break;

        default:
            Helpers::panic("Unimplemented G_13 instruction 0x%02x (decimal: %d) (full instr: 0x%08x)\n", (u32)instr.g_13_field, (u32)instr.g_13_field, instr.raw);
        }
        break;
    }
    case ORI:   ori(instr);     break;
    case ORIS:  oris(instr);    break;
    case XORI:  xori(instr);    break;
    case XORIS: xoris(instr);   break;
    case ANDI:  andi(instr);    break;
    case ANDIS: andis(instr);   break;
    case G_1E: {
        switch (instr.g_1e_field) {

        case RLDICL: rldicl(instr); break;

        default:
            Helpers::panic("Unimplemented G_1E instruction 0x%02x (decimal: %d) (full instr: 0x%08x)\n", (u32)instr.g_1e_field, (u32)instr.g_1e_field, instr.raw);
        }
        break;
    }
    case G_1F: {
        switch (instr.g_1f_field) {

        case CMPL:  cmpl(instr);    break;
        case MFSPR: mfspr(instr);   break;
        case OR:    or_(instr);     break;
        case MTSPR: mtspr(instr);   break;

        default:
            Helpers::panic("Unimplemented G_1F instruction 0x%03x (decimal: %d) (full instr: 0x%08x)\n", (u32)instr.g_1f_field, (u32)instr.g_1f_field, instr.raw);
        }
        break;
    }
    case LWZ:   lwz(instr);     break;
    case STW:   stw(instr);     break;
    case G_3A: {
        switch (instr.g_3a_field) {

        case LD:   ld(instr);     break;
        case LDU:  ldu(instr);    break;

        default:
            Helpers::panic("Unimplemented G_3A instruction 0x%02x (decimal: %d) (full instr: 0x%08x)\n", (u32)instr.g_3a_field, (u32)instr.g_3a_field, instr.raw);
        }
        break;
    }
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
void PPUInterpreter::cmpi(const Instruction& instr) {
    const s64 a = (instr.l) ? state.gprs[instr.ra] : (s64)(s32)state.gprs[instr.ra];
    const s64 b = (s64)(s16)instr.si;
    state.cr.compareAndUpdateCRField<s64>(instr.bf, a, b, 0);
}

void PPUInterpreter::addi(const Instruction& instr) {
    state.gprs[instr.rt] = (instr.ra ? state.gprs[instr.ra] : 0) + (s64)(s16)instr.si;
}

void PPUInterpreter::bc(const Instruction& instr) {
    const s32 sbd = (s32)(s16)(instr.bd << 2);
    if (instr.lk)  // Link
        state.lr = state.pc + 4;

    if (branchCondition(instr.bo, instr.bi)) {
        state.pc = (instr.aa == 1) ? sbd : state.pc + sbd;
    }
}

void PPUInterpreter::sc(const Instruction& instr) {
    ps3->syscall.doSyscall();
    state.pc -= 4;
}

void PPUInterpreter::b(const Instruction& instr) {
    const s32 sli = (s32)(instr.li << 8) >> 6;    // ((instr.li << 2) << 6) >> 6

    if (instr.lk)  // Link
        state.lr = state.pc + 4;

    state.pc = (instr.aa == 1) ? sli : state.pc + sli;
    state.pc -= 4;
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

void PPUInterpreter::stw(const Instruction& instr) {
    const s32 sd = (s32)(s16)instr.d;
    const u32 addr = (instr.ra == 0) ? sd : state.gprs[instr.ra] + sd;
    mem.write<u32>(addr, state.gprs[instr.rs]);
}

// G_13

void PPUInterpreter::bcctr(const Instruction& instr) {
    if (instr.lk)  // Link
        state.lr = state.pc + 4;

    if (branchCondition(instr.bo, instr.bi)) {
        state.pc = state.ctr - 4;
    }
}

// G_1E

void PPUInterpreter::rldicl(const Instruction& instr) {
    const auto sh = instr.sh_lo | (instr.sh_hi << 5);
    const auto mb = ((instr.mb & 1) << 5) | (instr.mb >> 1);
    const auto mask = 0xffffffffffffffffull >> mb;
    state.gprs[instr.ra] = std::rotl<u64>(state.gprs[instr.rs], sh) & mask;

    if (instr.rc)
        state.cr.compareAndUpdateCRField<s64>(0, state.gprs[instr.ra], 0);
}

// G_1F

void PPUInterpreter::cmpl(const Instruction& instr) {
    if (instr.l)
        state.cr.compareAndUpdateCRField<u64>(instr.bf, state.gprs[instr.ra], state.gprs[instr.rb], 0);
    else
        state.cr.compareAndUpdateCRField<u32>(instr.bf, state.gprs[instr.ra], state.gprs[instr.rb], 0);
}

void PPUInterpreter::mfspr(const Instruction& instr) {
    auto reversed_spr = ((instr.spr & 0x1f) << 5) | (instr.spr >> 5);
    switch (reversed_spr) {
    case 0b01000: state.gprs[instr.rt] = state.lr; break;
    default: Helpers::panic("mfspr: unimplemented register 0x%04x\n", reversed_spr);
    }
}

void PPUInterpreter::or_(const Instruction& instr) {
    state.gprs[instr.ra] = state.gprs[instr.rs] | state.gprs[instr.rb];
    if (instr.rc)
        state.cr.compareAndUpdateCRField<s64>(0, state.gprs[instr.ra], 0);
}

void PPUInterpreter::mtspr(const Instruction& instr) {
    auto reversed_spr = ((instr.spr & 0x1f) << 5) | (instr.spr >> 5);
    switch (reversed_spr) {
    case 0b01001: state.ctr = state.gprs[instr.rs]; break;
    default: Helpers::panic("mtspr: unimplemented register 0x%04x\n", reversed_spr);
    }
}

// G_3A

void PPUInterpreter::ld(const Instruction& instr) {
    const s32 sds = (s32)(s16)(instr.ds << 2);
    const u32 addr = (instr.ra == 0) ? sds : state.gprs[instr.ra] + sds;
    state.gprs[instr.rt] = mem.read<u64>(addr);
}

void PPUInterpreter::ldu(const Instruction& instr) {
    const s32 sds = (s32)(s16)(instr.ds << 2);
    const u32 addr = state.gprs[instr.ra] + sds;
    state.gprs[instr.rt] = mem.read<u64>(addr);
    state.gprs[instr.ra] = addr;    // Update
}

// G_3E

void PPUInterpreter::std(const Instruction& instr) {
    const s32 sds = (s32)(s16)(instr.ds << 2);
    const u32 addr = (instr.ra == 0) ? sds : state.gprs[instr.ra] + sds;
    mem.write<u64>(addr, state.gprs[instr.rs]);
}

void PPUInterpreter::stdu(const Instruction& instr) {
    const s32 sds = (s32)(s16)(instr.ds << 2);
    const u32 addr = state.gprs[instr.ra] + sds;
    mem.write<u64>(addr, state.gprs[instr.rs]);
    state.gprs[instr.ra] = addr;    // Update
}