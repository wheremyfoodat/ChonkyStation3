#include "PPUInterpreter.hpp"
#include <PlayStation3.hpp>


void PPUInterpreter::printFunctionCall() {
    auto symbol = ps3->elf_parser.getSymbol(state.pc);
    if (symbol.has_value())
        printf("[DEBUG] %s @ 0x%08llx\n", symbol.value().name.c_str(), state.pc);
}

void PPUInterpreter::step() {
    const u32 instrRaw = mem.read<u32>(state.pc);
    const Instruction instr { .raw = instrRaw };
    
    //PPUDisassembler::disasm(state, instr, &mem);

    switch (instr.opc) {
    
    case SUBFIC: subfic(instr);  break;
    case CMPLI:  cmpli(instr);   break;
    case CMPI:   cmpi(instr);    break;
    case ADDI:   addi(instr);    break;
    case ADDIS:  addis(instr);   break;
    case BC:     bc(instr);      break;
    case SC:     sc(instr);      break;
    case B:      b(instr);       break;
    case G_13: {
        switch (instr.g_13_field) {

        case BCLR:  bclr(instr);  break;
        case BCCTR: bcctr(instr); break;

        default:
            Helpers::panic("Unimplemented G_13 instruction 0x%02x (decimal: %d) (full instr: 0x%08x)\n", (u32)instr.g_13_field, (u32)instr.g_13_field, instr.raw);
        }
        break;
    }
    case RLWINM:    rlwinm(instr);  break;
    case ORI:       ori(instr);     break;
    case ORIS:      oris(instr);    break;
    case XORI:      xori(instr);    break;
    case XORIS:     xoris(instr);   break;
    case ANDI:      andi(instr);    break;
    case ANDIS:     andis(instr);   break;
    case G_1E: {
        switch (instr.g_1e_field) {

        case RLDICL: rldicl(instr); break;
        case RLDICR: rldicr(instr); break;

        default:
            Helpers::panic("Unimplemented G_1E instruction 0x%02x (decimal: %d) (full instr: 0x%08x)\n", (u32)instr.g_1e_field, (u32)instr.g_1e_field, instr.raw);
        }
        break;
    }
    case G_1F: {
        switch (instr.g_1f_field) {

        case CMP:       cmp(instr);     break;
        case MFCR:      mfcr(instr);    break;
        case CNTLZW:    cntlzw(instr);  break;
        case SLD:       sld(instr);     break;
        case AND:       and_(instr);    break;
        case CMPL:      cmpl(instr);    break;
        case SUBF:      subf(instr);    break;
        case ANDC:      andc(instr);    break;
        case NEG:       neg(instr);     break;
        case MTCRF:     mtcrf(instr);   break;
        case STDX:      stdx(instr);    break;
        case ADDZE:     addze(instr);   break;
        case MULLD:     mulld(instr);   break;
        case ADD:       add(instr);     break;
        case XOR:       xor_(instr);    break;
        case MFSPR:     mfspr(instr);   break;
        case OR:        or_(instr);     break;
        case DIVDU:     divdu(instr);   break;
        case MTSPR:     mtspr(instr);   break;
        case SRAWI:     srawi(instr);   break;
        case SRADI1:
        case SRADI2:    sradi(instr);   break;
        case EXTSH:     extsh(instr);   break;
        case EXTSW:     extsw(instr);   break;

        default:
            Helpers::panic("Unimplemented G_1F instruction 0x%03x (decimal: %d) (full instr: 0x%08x) @ 0x%016llx\n", (u32)instr.g_1f_field, (u32)instr.g_1f_field, instr.raw, state.pc);
        }
        break;
    }
    case LWZ:   lwz(instr);     break;
    case LBZ:   lbz(instr);     break;
    case LBZU:  lbzu(instr);    break;
    case STW:   stw(instr);     break;
    case STWU:  stwu(instr);    break;
    case STB:   stb(instr);     break;
    case STBU:  stbu(instr);    break;
    case LHZ:   lhz(instr);     break;
    case STH:   sth(instr);     break;
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
        Helpers::panic("Unimplemented opcode 0x%02x (decimal: %d) (full instr: 0x%08x) @ 0x%016llx\n", (u32)instr.opc, (u32)instr.opc, instr.raw, state.pc);
    }

    state.pc += 4;
}

// Main

void PPUInterpreter::subfic(const Instruction& instr) {
    const auto a = state.gprs[instr.ra];
    const s64 b = (s64)(s16)instr.si;
    const auto res = ~a + b + 1;
    state.xer.ca = res < a;
    state.gprs[instr.rt] = res;
}

void PPUInterpreter::cmpli(const Instruction& instr) {
    const u64 a = state.gprs[instr.ra];
    const u64 b = instr.ui;
    if (instr.l)
        state.cr.compareAndUpdateCRField<u64>(instr.bf, a, b, 0);
    else
        state.cr.compareAndUpdateCRField<u32>(instr.bf, a, b, 0);
}

void PPUInterpreter::cmpi(const Instruction& instr) {
    const s64 a = (s64)state.gprs[instr.ra];
    const s64 b = (s64)(s16)instr.si;
    if (instr.l)
        state.cr.compareAndUpdateCRField<s64>(instr.bf, a, b, 0);
    else
        state.cr.compareAndUpdateCRField<s32>(instr.bf, a, b, 0);
}

void PPUInterpreter::addi(const Instruction& instr) {
    state.gprs[instr.rt] = (instr.ra ? state.gprs[instr.ra] : 0) + (s64)(s16)instr.si;
}

void PPUInterpreter::addis(const Instruction& instr) {
    state.gprs[instr.rt] = (instr.ra ? state.gprs[instr.ra] : 0) + (s64)(s32)(instr.si << 16);
}

void PPUInterpreter::bc(const Instruction& instr) {
    const s64 sbd = (s64)(s16)(instr.bd << 2);
    if (instr.lk)  // Link
        state.lr = state.pc + 4;

    if (branchCondition(instr.bo, instr.bi)) {
        state.pc = (instr.aa == 1) ? sbd : (state.pc + sbd);
        printFunctionCall();
        state.pc -= 4;
    }
}

void PPUInterpreter::sc(const Instruction& instr) {
    ps3->syscall.doSyscall(true);
}

void PPUInterpreter::b(const Instruction& instr) {
    const s64 sli = (s64)(s32)(instr.li << 8) >> 6;    // ((instr.li << 2) << 6) >> 6

    if (instr.lk)  // Link
        state.lr = state.pc + 4;

    state.pc = (instr.aa == 1) ? sli : (state.pc + sli);
    printFunctionCall();
    state.pc -= 4;
}

void PPUInterpreter::rlwinm(const Instruction& instr) {
    const auto mask = rotationMask(instr.mb_5, instr.me_5);
    //Helpers::debugAssert(instr.mb_5 < instr.me_5, "rlwinm: mb > me");
    state.gprs[instr.ra] = std::rotl<u32>(state.gprs[instr.rs], instr.sh) & mask;
    if (instr.rc)
        state.cr.compareAndUpdateCRField<s32>(0, state.gprs[instr.ra], 0);
}

void PPUInterpreter::ori(const Instruction& instr) {
    state.gprs[instr.ra] = state.gprs[instr.rs] | instr.ui;
}

void PPUInterpreter::oris(const Instruction& instr) {
    state.gprs[instr.ra] = state.gprs[instr.rs] | ((u64)instr.ui << 16u);
}

void PPUInterpreter::xori(const Instruction& instr) {
    state.gprs[instr.ra] = state.gprs[instr.rs] ^ instr.ui;
}

void PPUInterpreter::xoris(const Instruction& instr) {
    state.gprs[instr.ra] = state.gprs[instr.rs] ^ ((u64)instr.ui << 16u);
}

void PPUInterpreter::andi(const Instruction& instr) {
    state.gprs[instr.ra] = state.gprs[instr.rs] & instr.ui;
}

void PPUInterpreter::andis(const Instruction& instr) {
    state.gprs[instr.ra] = state.gprs[instr.rs] & ((u64)instr.ui << 16u);
}

void PPUInterpreter::lwz(const Instruction& instr) {
    const s32 sd = (s32)(s16)instr.d;
    const u32 addr = (instr.ra == 0) ? sd : state.gprs[instr.ra] + sd;
    state.gprs[instr.rt] = mem.read<u32>(addr);
}

void PPUInterpreter::lbz(const Instruction& instr) {
    const s32 sd = (s32)(s16)instr.d;
    const u32 addr = (instr.ra == 0) ? sd : state.gprs[instr.ra] + sd;
    state.gprs[instr.rt] = mem.read<u8>(addr);
}

void PPUInterpreter::lbzu(const Instruction& instr) {
    const s32 sd = (s32)(s16)instr.d;
    const u32 addr = state.gprs[instr.ra] + sd;
    state.gprs[instr.rt] = mem.read<u8>(addr);
    state.gprs[instr.ra] = addr;    // Update
}

void PPUInterpreter::stw(const Instruction& instr) {
    const s32 sd = (s32)(s16)instr.d;
    const u32 addr = (instr.ra == 0) ? sd : state.gprs[instr.ra] + sd;
    mem.write<u32>(addr, state.gprs[instr.rs]);
}

void PPUInterpreter::stwu(const Instruction& instr) {
    const s32 sd = (s32)(s16)instr.d;
    const u32 addr = (instr.ra == 0) ? sd : state.gprs[instr.ra] + sd;
    mem.write<u32>(addr, state.gprs[instr.rs]);
}

void PPUInterpreter::stb(const Instruction& instr) {
    const s32 sd = (s32)(s16)instr.d;
    const u32 addr = (instr.ra == 0) ? sd : state.gprs[instr.ra] + sd;
    mem.write<u8>(addr, state.gprs[instr.rs]);
}

void PPUInterpreter::stbu(const Instruction& instr) {
    const s32 sd = (s32)(s16)instr.d;
    const u32 addr = (s32)state.gprs[instr.ra] + sd;
    mem.write<u8>(addr, state.gprs[instr.rs]);
    state.gprs[instr.ra] = addr;    // Update
}

void PPUInterpreter::lhz(const Instruction& instr) {
    const s32 sd = (s32)(s16)instr.d;
    const u32 addr = (instr.ra == 0) ? sd : state.gprs[instr.ra] + sd;
    state.gprs[instr.rt] = mem.read<u16>(addr);
}

void PPUInterpreter::sth(const Instruction& instr) {
    const s32 sd = (s32)(s16)instr.d;
    const u32 addr = (instr.ra == 0) ? sd : state.gprs[instr.ra] + sd;
    mem.write<u16>(addr, state.gprs[instr.rs]);
}

// G_13

void PPUInterpreter::bclr(const Instruction& instr) {
    auto lr = state.lr;
    if (instr.lk)  // Link
        state.lr = state.pc + 4;

    if (branchCondition(instr.bo, instr.bi)) {
        state.pc = lr;
        printFunctionCall();
        state.pc -= 4;
    }
}

void PPUInterpreter::bcctr(const Instruction& instr) {
    if (instr.lk)  // Link
        state.lr = state.pc + 4;

    if (branchCondition(instr.bo, instr.bi)) {
        state.pc = state.ctr;
        printFunctionCall();
        state.pc -= 4;
    }
}

// G_1E

void PPUInterpreter::rldicl(const Instruction& instr) {
    const auto sh = instr.sh_lo | (instr.sh_hi << 5);
    const auto mb = ((instr.mb_6 & 1) << 5) | (instr.mb_6 >> 1);
    const auto mask = 0xffffffffffffffffull >> mb;
    state.gprs[instr.ra] = std::rotl<u64>(state.gprs[instr.rs], sh) & mask;

    if (instr.rc)
        state.cr.compareAndUpdateCRField<s64>(0, state.gprs[instr.ra], 0);
}

void PPUInterpreter::rldicr(const Instruction& instr) {
    const auto sh = instr.sh_lo | (instr.sh_hi << 5);
    const auto me = ((instr.me_6 & 1) << 5) | (instr.me_6 >> 1);
    const auto mask = 0xffffffffffffffffull << (63 - me);
    state.gprs[instr.ra] = std::rotl<u64>(state.gprs[instr.rs], sh) & mask;
    if (instr.rc)
        state.cr.compareAndUpdateCRField<s64>(0, state.gprs[instr.ra], 0);
}

// G_1F

void PPUInterpreter::cmp(const Instruction& instr) {
    if (instr.l)
        state.cr.compareAndUpdateCRField<s64>(instr.bf, state.gprs[instr.ra], state.gprs[instr.rb], 0);
    else
        state.cr.compareAndUpdateCRField<s32>(instr.bf, state.gprs[instr.ra], state.gprs[instr.rb], 0);
}

void PPUInterpreter::mfcr(const Instruction& instr) {
    if (instr.one) {    // mfocrf
        auto n = 0;
        auto count = 0;
        for (int i = 0; i < 8; i++) {
            if ((instr.fxm >> i) & 1) {
                n = i;
                count++;
            }
        }
        Helpers::debugAssert(count == 1, "mfocrf with count != 1\n");

        state.gprs[instr.rt] = (u64)state.cr.getCRField(n) << (28 - (n * 4)); // ?
    }
    else {  // mfcrf
        state.gprs[instr.rt] = state.cr.raw;
    }
}

void PPUInterpreter::cntlzw(const Instruction& instr) {
    u8 n = 0;
    for (int i = 31; i >= 0; i--) {
        if ((state.gprs[instr.rs] >> i) & 1)
            break;
        else
            n++;
    }
    state.gprs[instr.ra] = n;
    // TODO: what does rc do?
    
}

void PPUInterpreter::sld(const Instruction& instr) {
    state.gprs[instr.ra] = state.gprs[instr.rs] << (state.gprs[instr.rb] & 0x7f);
    if (instr.rc)
        state.cr.compareAndUpdateCRField<s64>(0, state.gprs[instr.ra], 0);
}

void PPUInterpreter::and_(const Instruction& instr) {
    state.gprs[instr.ra] = state.gprs[instr.rs] & state.gprs[instr.rb];
    if (instr.rc)
        state.cr.compareAndUpdateCRField<s64>(0, state.gprs[instr.ra], 0);
}

void PPUInterpreter::cmpl(const Instruction& instr) {
    if (instr.l)
        state.cr.compareAndUpdateCRField<u64>(instr.bf, state.gprs[instr.ra], state.gprs[instr.rb], 0);
    else
        state.cr.compareAndUpdateCRField<u32>(instr.bf, state.gprs[instr.ra], state.gprs[instr.rb], 0);
}

void PPUInterpreter::subf(const Instruction& instr) {
    Helpers::debugAssert(!instr.oe, "subf: oe bit set\n");

    state.gprs[instr.rt] = state.gprs[instr.rb] - state.gprs[instr.ra];
    if (instr.rc)
        state.cr.compareAndUpdateCRField<s64>(0, state.gprs[instr.rt], 0);
}

void PPUInterpreter::andc(const Instruction& instr) {
    state.gprs[instr.ra] = state.gprs[instr.rs] & ~state.gprs[instr.rb];
    if (instr.rc)
        state.cr.compareAndUpdateCRField<s64>(0, state.gprs[instr.ra], 0);
}

void PPUInterpreter::neg(const Instruction& instr) {
    Helpers::debugAssert(!instr.oe, "neg: oe bit set\n");

    state.gprs[instr.rt] = -state.gprs[instr.ra];
    if (instr.rc)
        state.cr.compareAndUpdateCRField<s64>(0, state.gprs[instr.rt], 0);
}

void PPUInterpreter::mtcrf(const Instruction& instr) {
    if (instr.one) {    // mtocrf
        auto n = 0;
        auto count = 0;
        for (int i = 0; i < 8; i++) {
            if ((instr.fxm >> i) & 1) {
                n = i;
                count++;
            }
        }
        Helpers::debugAssert(count == 1, "mtocrf with count != 1\n");

        state.cr.setCRField(n, (state.gprs[instr.rt] >> (28 - (4 * n))) & 0xf); // TODO: double check this maybe but I think it's right
    }
    else {  // mtcrf
        Helpers::panic("Unimplemented mtcrf\n");
    }
}

void PPUInterpreter::stdx(const Instruction& instr) {
    mem.write<u64>(instr.ra ? (state.gprs[instr.ra] + state.gprs[instr.rb]) : state.gprs[instr.rb], state.gprs[instr.rs]);
}

void PPUInterpreter::addze(const Instruction& instr) {
    Helpers::debugAssert(!instr.oe, "add: oe bit set\n");

    const auto a = state.gprs[instr.ra];
    const u8 b = state.xer.ca;
    const auto res = a + b;
    state.gprs[instr.rt] = res;
    state.xer.ca = res < a;

    if (instr.rc)
        state.cr.compareAndUpdateCRField<s64>(0, state.gprs[instr.rt], 0);
}

void PPUInterpreter::mulld(const Instruction& instr) {
    state.gprs[instr.rt] = (s64)state.gprs[instr.ra] * (s64)state.gprs[instr.rb];
    if (instr.rc)
        state.cr.compareAndUpdateCRField<s64>(0, state.gprs[instr.rt], 0);
}

void PPUInterpreter::add(const Instruction& instr) {
    Helpers::debugAssert(!instr.oe, "add: oe bit set\n");

    state.gprs[instr.rt] = state.gprs[instr.ra] + state.gprs[instr.rb];
    if (instr.rc)
        state.cr.compareAndUpdateCRField<s64>(0, state.gprs[instr.rt], 0);
}

void PPUInterpreter::xor_(const Instruction& instr) {
    state.gprs[instr.ra] = state.gprs[instr.rs] ^ state.gprs[instr.rb];
    if (instr.rc)
        state.cr.compareAndUpdateCRField<s64>(0, state.gprs[instr.ra], 0);
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

void PPUInterpreter::divdu(const Instruction& instr) {
    const auto a = state.gprs[instr.ra];
    const auto b = state.gprs[instr.rb];
    Helpers::debugAssert(!instr.oe, "divdu: oe bit set\n");
    Helpers::debugAssert(b != 0, "divdu: division by 0\n");

    state.gprs[instr.rt] = a / b;
    if (instr.rc)
        state.cr.compareAndUpdateCRField<s64>(0, state.gprs[instr.rt], 0);
}

void PPUInterpreter::mtspr(const Instruction& instr) {
    auto reversed_spr = ((instr.spr & 0x1f) << 5) | (instr.spr >> 5);
    switch (reversed_spr) {
    case 0b01001: state.ctr = state.gprs[instr.rs]; break;
    case 0b01000: state.lr =  state.gprs[instr.rs]; break;
    default: Helpers::panic("mtspr: unimplemented register 0x%04x\n", reversed_spr);
    }
}

void PPUInterpreter::srawi(const Instruction& instr) {
    const auto sh = instr.sh_lo;
    state.gprs[instr.ra] = (s32)state.gprs[instr.rs] >> sh;
    state.xer.ca = ((s32)state.gprs[instr.rs] < 0) && (((u32)state.gprs[instr.ra] << sh) != (u32)state.gprs[instr.rs]);
    if (instr.rc)
        state.cr.compareAndUpdateCRField<s64>(0, state.gprs[instr.ra], 0);
}

void PPUInterpreter::sradi(const Instruction& instr) {
    const auto sh = instr.sh_lo | (instr.sh_hi << 5);
    state.gprs[instr.ra] = (s64)state.gprs[instr.rs] >> sh;
    state.xer.ca = ((s64)state.gprs[instr.rs] < 0) && ((state.gprs[instr.ra] << sh) != state.gprs[instr.rs]);
    if (instr.rc)
        state.cr.compareAndUpdateCRField<s64>(0, state.gprs[instr.ra], 0);
}

void PPUInterpreter::extsh(const Instruction& instr) {
    state.gprs[instr.ra] = (s64)(s16)state.gprs[instr.rs];
    if (instr.rc)
        state.cr.compareAndUpdateCRField<s64>(0, state.gprs[instr.ra], 0);
}

void PPUInterpreter::extsw(const Instruction& instr) {
    state.gprs[instr.ra] = (s64)(s32)state.gprs[instr.rs];
    if (instr.rc)
        state.cr.compareAndUpdateCRField<s64>(0, state.gprs[instr.ra], 0);
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