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
    

    switch (instr.opc) {
    
    case G_04: {
        switch (instr.g_04_field & 0x3f) {

        case VMADDFP:   vmaddfp(instr);     break;
        case VNMSUBFP:  vnmsubfp(instr);    break;
        case VSEL:      vsel(instr);        break;
        case VPERM:     vperm(instr);       break;
        case VSLDOI:    vsldoi(instr);      break;

        default:
            switch (instr.g_04_field) {

            case VCMPEQUB:  vcmpequb(instr);    break;
            case VADDFP:    vaddfp(instr);      break;
            case VSUBFP:    vsubfp(instr);      break;
            case VADDUWM:   vadduwm(instr);     break;
            case VCMPEQUW:  vcmpequw(instr);    break;
            case VMRGHW:    vmrghw(instr);      break;
            case VREFP:     vrefp(instr);       break;
            case VRSQRTEFP: vrsqrtefp(instr);   break;
            case VSLW:      vslw(instr);        break;
            case VMRGLW:    vmrglw(instr);      break;
            case VSPLTW:    vspltw(instr);      break;
            case VCFSX:     vcfsx(instr);       break;
            case VSPLTISW:  vspltisw(instr);    break;
            case VCTSXS:    vctsxs(instr);      break;
            case VAND:      vand(instr);        break;
            case VOR:       vor(instr);         break;
            case VXOR:      vxor(instr);        break;

            default:
                Helpers::panic("Unimplemented G_04 instruction 0x%02x (decimal: %d) (full instr: 0x%08x) @ 0x%016llx\n", (u32)instr.g_04_field, (u32)instr.g_04_field, instr.raw, state.pc);
            }
        }
        break;
    }
    case MULLI:  mulli(instr);   break;
    case SUBFIC: subfic(instr);  break;
    case CMPLI:  cmpli(instr);   break;
    case CMPI:   cmpi(instr);    break;
    case ADDIC:  addic(instr);    break;
    case ADDI:   addi(instr);    break;
    case ADDIS:  addis(instr);   break;
    case BC:     bc(instr);      break;
    case SC:     sc(instr);      break;
    case B:      b(instr);       break;
    case G_13: {
        switch (instr.g_13_field) {

        case MCRF:  mcrf(instr);    break;
        case BCLR:  bclr(instr);    break;
        case CROR:  cror(instr);    break;
        case BCCTR: bcctr(instr);   break;

        default:
            Helpers::panic("Unimplemented G_13 instruction 0x%02x (decimal: %d) (full instr: 0x%08x) @ 0x%016llx\n", (u32)instr.g_13_field, (u32)instr.g_13_field, instr.raw, state.pc);
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
        case RLDIC:  rldic(instr);  break;
        case RLDIMI: rldimi(instr); break;

        default:
            Helpers::panic("Unimplemented G_1E instruction 0x%02x (decimal: %d) (full instr: 0x%08x)\n", (u32)instr.g_1e_field, (u32)instr.g_1e_field, instr.raw);
        }
        break;
    }
    case G_1F: {
        switch (instr.g_1f_field) {

        case CMP:       cmp(instr);     break;
        case LVSL:      lvsl(instr);    break;
        case MULHDU:    mulhdu(instr);  break;
        case MFCR:      mfcr(instr);    break;
        case LDX:       ldx(instr);     break;
        case LWZX:      lwzx(instr);    break;
        case CNTLZW:    cntlzw(instr);  break;
        case SLW:       slw(instr);     break;
        case SLD:       sld(instr);     break;
        case AND:       and_(instr);    break;
        case CMPL:      cmpl(instr);    break;
        case LVSR:      lvsr(instr);    break;
        case SUBF:      subf(instr);    break;
        case CNTLZD:    cntlzd(instr);  break;
        case ANDC:      andc(instr);    break;
        case LBZX:      lbzx(instr);    break;
        case LVX:       lvx(instr);     break;
        case NEG:       neg(instr);     break;
        case NOR:       nor(instr);     break;
        case SUBFE:     subfe(instr);   break;
        case MTCRF:     mtcrf(instr);   break;
        case STDX:      stdx(instr);    break;
        case ADDZE:     addze(instr);   break;
        case STVX:      stvx(instr);    break;
        case MULLD:     mulld(instr);   break;
        case MULLW:     mullw(instr);   break;
        case ADD:       add(instr);     break;
        case DCBT:      break;
        case XOR:       xor_(instr);    break;
        case MFSPR:     mfspr(instr);   break;
        case OR:        or_(instr);     break;
        case DIVDU:     divdu(instr);   break;
        case DIVWU:     divwu(instr);   break;
        case MTSPR:     mtspr(instr);   break;
        case SRW:       srw(instr);     break;
        case SYNC:      break;
        case LFDX:      lfdx(instr);    break;
        case SRAWI:     srawi(instr);   break;
        case SRADI1:
        case SRADI2:    sradi(instr);   break;
        case EXTSH:     extsh(instr);   break;
        case EXTSB:     extsb(instr);   break;
        case EXTSW:     extsw(instr);   break;
        case STFIWX:    stfiwx(instr);  break;

        default:
            Helpers::panic("Unimplemented G_1F instruction 0x%03x (decimal: %d) (full instr: 0x%08x) @ 0x%016llx\n", (u32)instr.g_1f_field, (u32)instr.g_1f_field, instr.raw, state.pc);
        }
        break;
    }
    case LWZ:   lwz(instr);     break;
    case LWZU:  lwzu(instr);    break;
    case LBZ:   lbz(instr);     break;
    case LBZU:  lbzu(instr);    break;
    case STW:   stw(instr);     break;
    case STWU:  stwu(instr);    break;
    case STB:   stb(instr);     break;
    case STBU:  stbu(instr);    break;
    case LHZ:   lhz(instr);     break;
    case STH:   sth(instr);     break;
    case LFS:   lfs(instr);     break;
    case LFD:   lfd(instr);     break;
    case STFS:  stfs(instr);    break;
    case STFD:  stfd(instr);    break;
    case G_3A: {
        switch (instr.g_3a_field) {

        case LD:    ld(instr);      break;
        case LDU:   ldu(instr);     break;
        case LWA:   lwa(instr);     break;

        default:
            Helpers::panic("Unimplemented G_3A instruction 0x%02x (decimal: %d) (full instr: 0x%08x)\n", (u32)instr.g_3a_field, (u32)instr.g_3a_field, instr.raw);
        }
        break;
    }
    case G_3B: {
        switch (instr.g_3b_field) {

        case FDIVS:     fdivs(instr);   break;
        case FSUBS:     fsubs(instr);   break;
        case FADDS:     fadds(instr);   break;
        case FMULS:     fmuls(instr);   break;
        case FMADDS:    fmadds(instr);  break;

        default:
            Helpers::panic("Unimplemented G_3B instruction 0x%02x (decimal: %d) (full instr: 0x%08x)\n", (u32)instr.g_3b_field, (u32)instr.g_3b_field, instr.raw);
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
    case G_3F: {
        switch (instr.g_3f_field & 0x1f) {

        case FMUL:  fmul(instr);    break;
        case FMADD: fmadd(instr);   break;

        default:
            switch (instr.g_3f_field) {

            case FCMPU:     fcmpu(instr);   break;
            case FRSP:      frsp(instr);    break;
            case FCTIWZ:    fctiwz(instr);  break;
            case FDIV:      fdiv(instr);    break;
            case FSUB:      fsub(instr);    break;
            case FADD:      fadd(instr);    break;
            case FMR:       fmr(instr);     break;
            case FNEG:      fneg(instr);    break;
            case FCFID:     fcfid(instr);   break;

            default:
                Helpers::panic("Unimplemented G_3F instruction 0x%02x (decimal: %d) (full instr: 0x%08x) @ 0x%016llx\n", (u32)instr.g_3f_field, (u32)instr.g_3f_field, instr.raw, state.pc);
            }
        }
        break;
    }

    default:
        Helpers::panic("Unimplemented opcode 0x%02x (decimal: %d) (full instr: 0x%08x) @ 0x%016llx\n", (u32)instr.opc, (u32)instr.opc, instr.raw, state.pc);
    }

    state.pc += 4;
}

// Main

void PPUInterpreter::mulli(const Instruction& instr) {
    const s64 si = (s64)(s16)instr.si;
    state.gprs[instr.rt] = (s64)state.gprs[instr.ra] * si;
}

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

void PPUInterpreter::addic(const Instruction& instr) {
    const auto a = state.gprs[instr.ra];
    const s64 b = (s64)(s16)instr.si;
    const auto res = a + b;
    state.xer.ca = res < a;
    state.gprs[instr.rt] = res;
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

void PPUInterpreter::lwzu(const Instruction& instr) {
    const s32 sd = (s32)(s16)instr.d;
    const u32 addr = state.gprs[instr.ra] + sd;
    state.gprs[instr.rt] = mem.read<u32>(addr);
    state.gprs[instr.ra] = addr;    // Update
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
    const u32 addr = state.gprs[instr.ra] + sd;
    mem.write<u32>(addr, state.gprs[instr.rs]);
    state.gprs[instr.ra] = addr;    // Update
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

void PPUInterpreter::lfs(const Instruction& instr) {
    const s32 sd = (s32)(s16)instr.d;
    const u32 addr = (instr.ra == 0) ? sd : state.gprs[instr.ra] + sd;
    u32 v = mem.read<u32>(addr);
    state.fprs[instr.frt] = reinterpret_cast<float&>(v);
}

void PPUInterpreter::lfd(const Instruction& instr) {
    const s32 sd = (s32)(s16)instr.d;
    const u32 addr = (instr.ra == 0) ? sd : state.gprs[instr.ra] + sd;
    u64 v = mem.read<u64>(addr);
    state.fprs[instr.frt] = reinterpret_cast<double&>(v);
}

void PPUInterpreter::stfs(const Instruction& instr) {
    const s32 sd = (s32)(s16)instr.d;
    const u32 addr = (instr.ra == 0) ? sd : state.gprs[instr.ra] + sd;
    float v = (float)state.fprs[instr.frs];
    mem.write<u32>(addr, reinterpret_cast<u32&>(v));
}

void PPUInterpreter::stfd(const Instruction& instr) {
    const s32 sd = (s32)(s16)instr.d;
    const u32 addr = (instr.ra == 0) ? sd : state.gprs[instr.ra] + sd;
    mem.write<u64>(addr, reinterpret_cast<u64&>(state.fprs[instr.frs]));
}

// G_04

void PPUInterpreter::vcmpequb(const Instruction& instr) {
    u8 all_equal = 0x8;
    u8 none_equal = 0x2;

    for (int i = 0; i < 16; i++) {
        if (state.vrs[instr.va].b[i] == state.vrs[instr.vb].b[i]) {
            state.vrs[instr.vd].b[i] = 0xff;
            none_equal = 0;
        }
        else {
            state.vrs[instr.vd].b[i] = 0;
            all_equal = 0;
        }
    }

    if (instr.rc_v)
        state.cr.setCRField(6, all_equal | none_equal);
}

void PPUInterpreter::vsel(const Instruction& instr) {
    // Where vc is 0, we take va, so we invert the mask and do va & ~vc
    state.vrs[instr.vd].dw[0] = (state.vrs[instr.va].dw[0] & ~state.vrs[instr.vc].dw[0]) | (state.vrs[instr.vb].dw[0] & state.vrs[instr.vc].dw[0]);
    state.vrs[instr.vd].dw[1] = (state.vrs[instr.va].dw[1] & ~state.vrs[instr.vc].dw[1]) | (state.vrs[instr.vb].dw[1] & state.vrs[instr.vc].dw[1]);
}

void PPUInterpreter::vperm(const Instruction& instr) {
    u8 src[32];
    for (int i = 0; i < 16; i++) src[i +  0] = state.vrs[instr.vb].b[i];
    for (int i = 0; i < 16; i++) src[i + 16] = state.vrs[instr.va].b[i];
    
    // The index is in the usual PPC reversed byte order, so we need to flip it with 31 - 
    for (int i = 0; i < 16; i++) {
        const auto idx = state.vrs[instr.vc].b[i] & 0x1f;
        state.vrs[instr.vd].b[i] = src[31 - idx];
    }
}

void PPUInterpreter::vsldoi(const Instruction& instr) {
    u8 src[32];
    for (int i = 0; i < 16; i++) src[i + 0] = state.vrs[instr.vb].b[i];
    for (int i = 0; i < 16; i++) src[i + 16] = state.vrs[instr.va].b[i];

    for (int i = 0; i < 16; i++) state.vrs[instr.vd].b[15 - i] = src[31 - (instr.shb + i)];
}

void PPUInterpreter::vmaddfp(const Instruction& instr) {
    state.vrs[instr.vd].f[0] = state.vrs[instr.va].f[0] * state.vrs[instr.vc].f[0] + state.vrs[instr.vb].f[0];
    state.vrs[instr.vd].f[1] = state.vrs[instr.va].f[1] * state.vrs[instr.vc].f[1] + state.vrs[instr.vb].f[1];
    state.vrs[instr.vd].f[2] = state.vrs[instr.va].f[2] * state.vrs[instr.vc].f[2] + state.vrs[instr.vb].f[2];
    state.vrs[instr.vd].f[3] = state.vrs[instr.va].f[3] * state.vrs[instr.vc].f[3] + state.vrs[instr.vb].f[3];
}

void PPUInterpreter::vnmsubfp(const Instruction& instr) {
    state.vrs[instr.vd].f[0] = -(state.vrs[instr.va].f[0] * state.vrs[instr.vc].f[0] - state.vrs[instr.vb].f[0]);
    state.vrs[instr.vd].f[1] = -(state.vrs[instr.va].f[1] * state.vrs[instr.vc].f[1] - state.vrs[instr.vb].f[1]);
    state.vrs[instr.vd].f[2] = -(state.vrs[instr.va].f[2] * state.vrs[instr.vc].f[2] - state.vrs[instr.vb].f[2]);
    state.vrs[instr.vd].f[3] = -(state.vrs[instr.va].f[3] * state.vrs[instr.vc].f[3] - state.vrs[instr.vb].f[3]);
}

void PPUInterpreter::vaddfp(const Instruction& instr) {
    state.vrs[instr.vd].f[0] = state.vrs[instr.va].f[0] + state.vrs[instr.vb].f[0];
    state.vrs[instr.vd].f[1] = state.vrs[instr.va].f[1] + state.vrs[instr.vb].f[1];
    state.vrs[instr.vd].f[2] = state.vrs[instr.va].f[2] + state.vrs[instr.vb].f[2];
    state.vrs[instr.vd].f[3] = state.vrs[instr.va].f[3] + state.vrs[instr.vb].f[3];
}

void PPUInterpreter::vsubfp(const Instruction& instr) {
    state.vrs[instr.vd].f[0] = state.vrs[instr.va].f[0] - state.vrs[instr.vb].f[0];
    state.vrs[instr.vd].f[1] = state.vrs[instr.va].f[1] - state.vrs[instr.vb].f[1];
    state.vrs[instr.vd].f[2] = state.vrs[instr.va].f[2] - state.vrs[instr.vb].f[2];
    state.vrs[instr.vd].f[3] = state.vrs[instr.va].f[3] - state.vrs[instr.vb].f[3];
}

void PPUInterpreter::vadduwm(const Instruction& instr) {
    state.vrs[instr.vd].w[0] = state.vrs[instr.va].w[0] + state.vrs[instr.vb].w[0];
    state.vrs[instr.vd].w[1] = state.vrs[instr.va].w[1] + state.vrs[instr.vb].w[1];
    state.vrs[instr.vd].w[2] = state.vrs[instr.va].w[2] + state.vrs[instr.vb].w[2];
    state.vrs[instr.vd].w[3] = state.vrs[instr.va].w[3] + state.vrs[instr.vb].w[3];
}

void PPUInterpreter::vcmpequw(const Instruction& instr) {
    u8 all_equal = 0x8;
    u8 none_equal = 0x2;

    for (int i = 0; i < 4; i++) {
        if (state.vrs[instr.va].w[i] == state.vrs[instr.vb].w[i]) {
            state.vrs[instr.vd].w[i] = 0xffffffff;
            none_equal = 0;
        }
        else {
            state.vrs[instr.vd].w[i] = 0;
            all_equal = 0;
        }
    }

    if (instr.rc_v)
        state.cr.setCRField(6, all_equal | none_equal);
}

void PPUInterpreter::vmrghw(const Instruction& instr) {
    const VR a = state.vrs[instr.va];
    const VR b = state.vrs[instr.vb];

    state.vrs[instr.vd].w[0] = b.w[2];
    state.vrs[instr.vd].w[1] = a.w[2];
    state.vrs[instr.vd].w[2] = b.w[3];
    state.vrs[instr.vd].w[3] = a.w[3];
}

void PPUInterpreter::vrefp(const Instruction& instr) {
    state.vrs[instr.vd].f[0] = 1.0f / state.vrs[instr.vb].f[0];
    state.vrs[instr.vd].f[1] = 1.0f / state.vrs[instr.vb].f[1];
    state.vrs[instr.vd].f[2] = 1.0f / state.vrs[instr.vb].f[2];
    state.vrs[instr.vd].f[3] = 1.0f / state.vrs[instr.vb].f[3];
}

void PPUInterpreter::vrsqrtefp(const Instruction& instr) {
    state.vrs[instr.vd].f[0] = 1.0f / sqrtf(state.vrs[instr.vb].f[0]);
    state.vrs[instr.vd].f[1] = 1.0f / sqrtf(state.vrs[instr.vb].f[1]);
    state.vrs[instr.vd].f[2] = 1.0f / sqrtf(state.vrs[instr.vb].f[2]);
    state.vrs[instr.vd].f[3] = 1.0f / sqrtf(state.vrs[instr.vb].f[3]);
}

void PPUInterpreter::vslw(const Instruction& instr) {
    state.vrs[instr.vd].w[0] = state.vrs[instr.va].w[0] << (state.vrs[instr.vb].w[0] & 0x1f);
    state.vrs[instr.vd].w[1] = state.vrs[instr.va].w[1] << (state.vrs[instr.vb].w[1] & 0x1f);
    state.vrs[instr.vd].w[2] = state.vrs[instr.va].w[2] << (state.vrs[instr.vb].w[2] & 0x1f);
    state.vrs[instr.vd].w[3] = state.vrs[instr.va].w[3] << (state.vrs[instr.vb].w[3] & 0x1f);
}

void PPUInterpreter::vmrglw(const Instruction& instr) {
    const VR a = state.vrs[instr.va];
    const VR b = state.vrs[instr.vb];

    state.vrs[instr.vd].w[0] = b.w[0];
    state.vrs[instr.vd].w[1] = a.w[0];
    state.vrs[instr.vd].w[2] = b.w[1];
    state.vrs[instr.vd].w[3] = a.w[1];
}

void PPUInterpreter::vspltw(const Instruction& instr) {
    Helpers::debugAssert(instr.uimm < 4, "spltw: uimm >= 4\n");

    const u32 v = state.vrs[instr.vb].w[3 - instr.uimm];

    state.vrs[instr.vd].w[0] = v;
    state.vrs[instr.vd].w[1] = v;
    state.vrs[instr.vd].w[2] = v;
    state.vrs[instr.vd].w[3] = v;
}

void PPUInterpreter::vcfsx(const Instruction& instr) {
    const auto factor = 1 << instr.uimm;
    state.vrs[instr.vd].f[0] = (float)(s32)state.vrs[instr.vb].w[0] / factor;
    state.vrs[instr.vd].f[1] = (float)(s32)state.vrs[instr.vb].w[1] / factor;
    state.vrs[instr.vd].f[2] = (float)(s32)state.vrs[instr.vb].w[2] / factor;
    state.vrs[instr.vd].f[3] = (float)(s32)state.vrs[instr.vb].w[3] / factor;
}

void PPUInterpreter::vspltisw(const Instruction& instr) {
    s32 si = (s32)((s8)(instr.simm << 3)) >> 3; // Sign extend 5 bit field

    state.vrs[instr.vd].w[0] = si;
    state.vrs[instr.vd].w[1] = si;
    state.vrs[instr.vd].w[2] = si;
    state.vrs[instr.vd].w[3] = si;
}

void PPUInterpreter::vctsxs(const Instruction& instr) {
    const auto factor = 1 << instr.uimm;
    for (int i = 0; i < 4; i++) {
        s64 v = (s64)(state.vrs[instr.vb].f[i] * factor);
        if (v > INT32_MAX) v = INT32_MAX;
        else if (v < INT32_MIN) v = INT32_MIN;
        state.vrs[instr.vd].w[i] = (s32)v;
    }
}

void PPUInterpreter::vand(const Instruction& instr) {
    state.vrs[instr.vd].dw[0] = state.vrs[instr.va].dw[0] & state.vrs[instr.vb].dw[0];
    state.vrs[instr.vd].dw[1] = state.vrs[instr.va].dw[1] & state.vrs[instr.vb].dw[1];
}

void PPUInterpreter::vor(const Instruction& instr) {
    state.vrs[instr.vd].dw[0] = state.vrs[instr.va].dw[0] | state.vrs[instr.vb].dw[0];
    state.vrs[instr.vd].dw[1] = state.vrs[instr.va].dw[1] | state.vrs[instr.vb].dw[1];
}

void PPUInterpreter::vxor(const Instruction& instr) {
    state.vrs[instr.vd].dw[0] = state.vrs[instr.va].dw[0] ^ state.vrs[instr.vb].dw[0];
    state.vrs[instr.vd].dw[1] = state.vrs[instr.va].dw[1] ^ state.vrs[instr.vb].dw[1];
}

// G_13

void PPUInterpreter::mcrf(const Instruction& instr) {
    state.cr.setCRField(instr.bf, state.cr.getCRField(instr.bfa));
}

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

void PPUInterpreter::cror(const Instruction& instr) {
    const auto a = (state.cr.raw >> instr.ba) & 1;
    const auto b = (state.cr.raw >> instr.bb) & 1;
    state.cr.raw &= ~(1 << instr.bt);
    state.cr.raw |= (a | b) << instr.bt;
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

void PPUInterpreter::rldic(const Instruction& instr) {
    const auto sh = instr.sh_lo | (instr.sh_hi << 5);
    const auto mb = ((instr.mb_6 & 1) << 5) | (instr.mb_6 >> 1);
    const auto mask = (0xffffffffffffffffull >> mb) & (0xffffffffffffffffull << sh);
    Helpers::debugAssert((63 - sh) > mb, "rldic: 63 - sh <= mb (invert mask?)\n");
    state.gprs[instr.ra] = std::rotl<u64>(state.gprs[instr.rs], sh) & mask;
    if (instr.rc)
        state.cr.compareAndUpdateCRField<s64>(0, state.gprs[instr.ra], 0);
}

void PPUInterpreter::rldimi(const Instruction& instr) {
    const auto sh = instr.sh_lo | (instr.sh_hi << 5);
    const auto mb = ((instr.mb_6 & 1) << 5) | (instr.mb_6 >> 1);
    const auto mask = (0xffffffffffffffffull >> mb) & (0xffffffffffffffffull << sh);
    Helpers::debugAssert((63 - sh) > mb, "rldic: 63 - sh <= mb (invert mask?)\n");
    state.gprs[instr.ra] = (state.gprs[instr.ra] & ~mask) | (std::rotl<u64>(state.gprs[instr.rs], sh) & mask);
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

void PPUInterpreter::lvsl(const Instruction& instr) {
    const u32 addr = instr.ra ? (state.gprs[instr.ra] + state.gprs[instr.rb]) : state.gprs[instr.rb];
    state.vrs[instr.vd].dw[0] = lvsl_shifts[addr & 0xf][0];
    state.vrs[instr.vd].dw[1] = lvsl_shifts[addr & 0xf][1];
}

void PPUInterpreter::mulhdu(const Instruction& instr) {
    const u64 a = state.gprs[instr.ra];
    const u64 b = state.gprs[instr.rb];
#ifdef _MSC_VER
    _umul128(a, b, &state.gprs[instr.rt]);
#else
    using uint128_t = unsigned __int128;

    uint128_t res = (uint128_t)a * b;
    state.gprs[instr.rt] = res >> 64;
#endif
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

void PPUInterpreter::ldx(const Instruction& instr) {
    state.gprs[instr.rt] = mem.read<u64>(instr.ra ? (state.gprs[instr.ra] + state.gprs[instr.rb]) : state.gprs[instr.rb]);
}

void PPUInterpreter::lwzx(const Instruction& instr) {
    state.gprs[instr.rt] = mem.read<u32>(instr.ra ? (state.gprs[instr.ra] + state.gprs[instr.rb]) : state.gprs[instr.rb]);
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

void PPUInterpreter::slw(const Instruction& instr) {
    state.gprs[instr.ra] = (u32)state.gprs[instr.rs] << (state.gprs[instr.rb] & 0x1f);
    if (instr.rc)
        state.cr.compareAndUpdateCRField<s32>(0, state.gprs[instr.ra], 0);
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

void PPUInterpreter::lvsr(const Instruction& instr) {
    const u32 addr = instr.ra ? (state.gprs[instr.ra] + state.gprs[instr.rb]) : state.gprs[instr.rb];
    state.vrs[instr.vd].dw[0] = lvsr_shifts[addr & 0xf][0];
    state.vrs[instr.vd].dw[1] = lvsr_shifts[addr & 0xf][1];
}

void PPUInterpreter::subf(const Instruction& instr) {
    Helpers::debugAssert(!instr.oe, "subf: oe bit set\n");

    state.gprs[instr.rt] = state.gprs[instr.rb] - state.gprs[instr.ra];
    if (instr.rc)
        state.cr.compareAndUpdateCRField<s64>(0, state.gprs[instr.rt], 0);
}

void PPUInterpreter::cntlzd(const Instruction& instr) {
    u8 n = 0;
    for (int i = 63; i >= 0; i--) {
        if ((state.gprs[instr.rs] >> i) & 1)
            break;
        else
            n++;
    }
    state.gprs[instr.ra] = n;
    // TODO: what does rc do?
}


void PPUInterpreter::andc(const Instruction& instr) {
    state.gprs[instr.ra] = state.gprs[instr.rs] & ~state.gprs[instr.rb];
    if (instr.rc)
        state.cr.compareAndUpdateCRField<s64>(0, state.gprs[instr.ra], 0);
}

void PPUInterpreter::lbzx(const Instruction& instr) {
    state.gprs[instr.rt] = mem.read<u8>(instr.ra ? (state.gprs[instr.ra] + state.gprs[instr.rb]) : state.gprs[instr.rb]);
}

void PPUInterpreter::lvx(const Instruction& instr) {
    const u32 addr = instr.ra ? (state.gprs[instr.ra] + state.gprs[instr.rb]) : state.gprs[instr.rb];
    //for (int i = 0; i < 16; i++)
    //  state.vrs[instr.vd].b[15 - i] = mem.read<u8>(addr + i);
    //for (int i = 0; i < 4; i++)
    //  state.vrs[instr.vd].w[i] = mem.read<u32>(addr + i * 4);
    state.vrs[instr.vd].dw[1] = mem.read<u64>(addr);
    state.vrs[instr.vd].dw[0] = mem.read<u64>(addr + 8);
}

void PPUInterpreter::neg(const Instruction& instr) {
    Helpers::debugAssert(!instr.oe, "neg: oe bit set\n");

    state.gprs[instr.rt] = -state.gprs[instr.ra];
    if (instr.rc)
        state.cr.compareAndUpdateCRField<s64>(0, state.gprs[instr.rt], 0);
}

void PPUInterpreter::nor(const Instruction& instr) {
    state.gprs[instr.ra] = ~(state.gprs[instr.rs] | state.gprs[instr.rb]);
    if (instr.rc)
        state.cr.compareAndUpdateCRField<s64>(0, state.gprs[instr.ra], 0);
}

void PPUInterpreter::subfe(const Instruction& instr) {
    Helpers::debugAssert(!instr.oe, "subfe: oe bit set\n");

    const auto a = state.gprs[instr.ra];
    const auto b = state.gprs[instr.rb];
    const auto res = ~a + b + state.xer.ca;
    state.xer.ca = res < a;
    state.gprs[instr.rt] = res;
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

void PPUInterpreter::stvx(const Instruction& instr) {
    const u32 addr = instr.ra ? (state.gprs[instr.ra] + state.gprs[instr.rb]) : state.gprs[instr.rb];
    mem.write<u64>(addr, state.vrs[instr.vs].dw[1]);
    mem.write<u64>(addr + 8, state.vrs[instr.vs].dw[0]);
}

void PPUInterpreter::mulld(const Instruction& instr) {
    Helpers::debugAssert(!instr.oe, "mulld: oe bit set\n");

    state.gprs[instr.rt] = (s64)state.gprs[instr.ra] * (s64)state.gprs[instr.rb];
    if (instr.rc)
        state.cr.compareAndUpdateCRField<s64>(0, state.gprs[instr.rt], 0);
}

void PPUInterpreter::mullw(const Instruction& instr) {
    Helpers::debugAssert(!instr.oe, "mullw: oe bit set\n");

    state.gprs[instr.rt] = (s64)((s32)state.gprs[instr.ra] * (s32)state.gprs[instr.rb]);
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

void PPUInterpreter::divwu(const Instruction& instr) {
    const auto a = state.gprs[instr.ra];
    const auto b = state.gprs[instr.rb];
    Helpers::debugAssert(!instr.oe, "divwu: oe bit set\n");
    Helpers::debugAssert(b != 0, "divwu: division by 0\n");

    state.gprs[instr.rt] = (u32)a / (u32)b;
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

void PPUInterpreter::srw(const Instruction& instr) {
    state.gprs[instr.ra] = (u32)state.gprs[instr.rs] >> (state.gprs[instr.rb] & 0x3f);
    if (instr.rc)
        state.cr.compareAndUpdateCRField<s64>(0, state.gprs[instr.ra], 0);
}

void PPUInterpreter::lfdx(const Instruction& instr) {
    u64 v = mem.read<u64>(instr.ra ? (state.gprs[instr.ra] + state.gprs[instr.rb]) : state.gprs[instr.rb]);
    state.fprs[instr.frt] = reinterpret_cast<double&>(v);
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

void PPUInterpreter::extsb(const Instruction& instr) {
    state.gprs[instr.ra] = (s64)(s8)state.gprs[instr.rs];
    if (instr.rc)
        state.cr.compareAndUpdateCRField<s64>(0, state.gprs[instr.ra], 0);
}

void PPUInterpreter::extsw(const Instruction& instr) {
    state.gprs[instr.ra] = (s64)(s32)state.gprs[instr.rs];
    if (instr.rc)
        state.cr.compareAndUpdateCRField<s64>(0, state.gprs[instr.ra], 0);
}

void PPUInterpreter::stfiwx(const Instruction& instr) {
    mem.write<u32>(instr.ra ? (state.gprs[instr.ra] + state.gprs[instr.rb]) : state.gprs[instr.rb], reinterpret_cast<u32&>(state.fprs[instr.frs]));
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

void PPUInterpreter::lwa(const Instruction& instr) {
    const s32 sd = (s32)(s16)instr.d;
    const u32 addr = (instr.ra == 0) ? sd : state.gprs[instr.ra] + sd;
    state.gprs[instr.rt] = (s64)(s32)mem.read<u32>(addr);
}

// G_3B

void PPUInterpreter::fdivs(const Instruction& instr) {
    // TODO: division by 0? NaNs? inf?
    Helpers::debugAssert(!instr.rc, "fdivs: rc\n");
    state.fprs[instr.frt] = (float)(state.fprs[instr.fra] / state.fprs[instr.frb]);
}

void PPUInterpreter::fsubs(const Instruction& instr) {
    Helpers::debugAssert(!instr.rc, "fsubs: rc\n");
    state.fprs[instr.frt] = (float)(state.fprs[instr.fra] - state.fprs[instr.frb]);
}

void PPUInterpreter::fadds(const Instruction& instr) {
    Helpers::debugAssert(!instr.rc, "fadds: rc\n");
    state.fprs[instr.frt] = (float)(state.fprs[instr.fra] + state.fprs[instr.frb]);
}

void PPUInterpreter::fmuls(const Instruction& instr) {
    Helpers::debugAssert(!instr.rc, "fmuls: rc\n");
    state.fprs[instr.frt] = (float)(state.fprs[instr.fra] * state.fprs[instr.frc]);
}

void PPUInterpreter::fmadds(const Instruction& instr) {
    Helpers::debugAssert(!instr.rc, "fmadds: rc\n");
    state.fprs[instr.frt] = (float)(state.fprs[instr.fra] * state.fprs[instr.frc] + state.fprs[instr.frb]);
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

// G_3F

void PPUInterpreter::fcmpu(const Instruction& instr) {
    state.cr.compareAndUpdateCRField<double>(instr.bf, state.fprs[instr.fra], state.fprs[instr.frb], 0);
}

void PPUInterpreter::frsp(const Instruction& instr) {
    Helpers::debugAssert(!instr.rc, "frsp: rc\n");
    state.fprs[instr.frt] = (float)state.fprs[instr.frb];
}

void PPUInterpreter::fctiwz(const Instruction& instr) {
    Helpers::debugAssert(!instr.rc, "fctiwz: rc\n");
    s64 v = (s64)state.fprs[instr.frb];
    if (v > INT32_MAX) v = 0x7fffffff;
    else if (v < INT32_MIN) v = 0x80000000;
    reinterpret_cast<u64&>(state.fprs[instr.frt]) = 0xfff8000000000000ull | v;
}

void PPUInterpreter::fdiv(const Instruction& instr) {
    // TODO: division by 0? NaNs? inf?
    Helpers::debugAssert(!instr.rc, "fdiv: rc\n");
    state.fprs[instr.frt] = state.fprs[instr.fra] / state.fprs[instr.frb];
}

void PPUInterpreter::fsub(const Instruction& instr) {
    Helpers::debugAssert(!instr.rc, "fsub: rc\n");
    state.fprs[instr.frt] = state.fprs[instr.fra] - state.fprs[instr.frb];
}

void PPUInterpreter::fadd(const Instruction& instr) {
    Helpers::debugAssert(!instr.rc, "fadd: rc\n");
    state.fprs[instr.frt] = state.fprs[instr.fra] + state.fprs[instr.frb];
}

void PPUInterpreter::fmr(const Instruction& instr) {
    Helpers::debugAssert(!instr.rc, "fmr: rc\n");
    state.fprs[instr.frt] = state.fprs[instr.frb];
}

void PPUInterpreter::fmul(const Instruction& instr) {
    Helpers::debugAssert(!instr.rc, "fmul: rc\n");
    state.fprs[instr.frt] = state.fprs[instr.fra] * state.fprs[instr.frc];
}

void PPUInterpreter::fmadd(const Instruction& instr) {
    Helpers::debugAssert(!instr.rc, "fmadd: rc\n");
    state.fprs[instr.frt] = state.fprs[instr.fra] * state.fprs[instr.frc] + state.fprs[instr.frb];
}

void PPUInterpreter::fneg(const Instruction& instr) {
    Helpers::debugAssert(!instr.rc, "fneg: rc\n");
    state.fprs[instr.frt] = -state.fprs[instr.frb];
}

void PPUInterpreter::fcfid(const Instruction& instr) {
    Helpers::debugAssert(!instr.rc, "fcfid: rc\n");
    state.fprs[instr.frt] = (double)reinterpret_cast<s64&>(state.fprs[instr.frb]);
}
