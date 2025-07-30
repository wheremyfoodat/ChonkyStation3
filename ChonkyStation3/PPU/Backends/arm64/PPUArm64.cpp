#include "PPUArm64.hpp"
#include <PlayStation3.hpp>

#define PC_OFFSET ((uintptr_t) &state.pc - (uintptr_t)this)
#define GPR_OFFSET(i) ((uintptr_t) &state.gprs[i] - (uintptr_t)this)
#define FPR_OFFSET(i) ((uintptr_t) &state.fprs[i] - (uintptr_t)this)
#define VPR_OFFSET(i) ((uintptr_t) &state.vprs[i] - (uintptr_t)this)

#define FALLBACK(instructionName, instructionRaw) \
    MOV(arg2, instructionRaw); \
    emitInterpreterFunctionCall(&PPUInterpreter::instructionName, X4);

PPUArm64::PPUArm64(Memory& mem, PlayStation3* ps3) : PPU(mem, ps3), interpreter(mem, ps3, stateInternal),
    oaknut::CodeBlock(alloc_size), oaknut::CodeGenerator(oaknut::CodeBlock::ptr()) {
    
    code_blocks = new JITCallback*[0x1'0000'0000ull / page_size]();
    emitDispatcher();
}

void PPUArm64::emitDispatcher() {
    align(16);
    dispatcher = xptr<JITCallback>();
    // TODO

    emitBlockLookup();
}

void PPUArm64::emitBlockLookup() {
    // TODO
}

void PPUArm64::writebackPC(u32 newPC) {
    MOV(scratch1, newPC);
    STR(scratch1, state_pointer, PC_OFFSET);
}

PPUArm64::JITCallback PPUArm64::compileBlock() {
    oaknut::CodeBlock::unprotect();
    
    align(16);
    JITCallback cb = xptr<JITCallback>();

    recompiler_pc = state.pc;
    block_size = 0;
    stop_compiling = false;

    // Push state pointer (x29) and LR (X30)
    STP(state_pointer, X30, SP, PRE_INDEXED, -16);
    // Make the state pointer point to the JIT object
    MOV(state_pointer, arg1);

    while (shouldContinue()) {
        const u32 instr_raw = mem.read<u32>(recompiler_pc);
        const Instruction instruction = { .raw = instr_raw };
        block_size++;

        // printf("%s\n", PPUDisassembler::disasm(state, instruction, &mem).c_str());
        compileInstruction(instruction);
        recompiler_pc += 4;
    }

    // Restore x29 and LR, return the number of instructions executed as the cycle count
    LDP(state_pointer, X30, SP, POST_INDEXED, 16);
    MOV(X0, block_size);
    RET();

    // TODO: Write back PC if the block ended because it got too long

    // Mark memory as RX and invalidate affect icache ranges before returning and executing the block
    oaknut::CodeBlock::protect();
    oaknut::CodeBlock::invalidate((u32*)cb, uintptr_t(xptr<u32*>()) - uintptr_t(cb));
    return cb;
}

int PPUArm64::step() {
    // TODO: Make lookup faster & port to assembly
    auto& page = code_blocks[state.pc >> page_shift];
    if (page == nullptr) {
        page = new JITCallback[page_size >> 2]();
    }

    auto& func = page[(state.pc & page_mask) >> 2];
    if (!func) {
        func = compileBlock();
    }

    return func(this);
}

void PPUArm64::bc(Instruction instr) {
    writebackPC(recompiler_pc);
    stop_compiling = true;
    FALLBACK(bc, instr.raw);
    LDR(scratch1, state_pointer, PC_OFFSET); ADD(scratch1, scratch1, 4); STR(scratch1, state_pointer, PC_OFFSET);
}

void PPUArm64::sc(Instruction instr) {
    writebackPC(recompiler_pc);
    stop_compiling = true;
    FALLBACK(sc, instr.raw);
    LDR(scratch1, state_pointer, PC_OFFSET); ADD(scratch1, scratch1, 4); STR(scratch1, state_pointer, PC_OFFSET);
}

void PPUArm64::b(Instruction instr) {
    writebackPC(recompiler_pc);
    stop_compiling = true;
    FALLBACK(b, instr.raw);
    LDR(scratch1, state_pointer, PC_OFFSET); ADD(scratch1, scratch1, 4); STR(scratch1, state_pointer, PC_OFFSET);
}

void PPUArm64::bclr(Instruction instr) {
    writebackPC(recompiler_pc);
    stop_compiling = true;
    FALLBACK(bclr, instr.raw);
    LDR(scratch1, state_pointer, PC_OFFSET); ADD(scratch1, scratch1, 4); STR(scratch1, state_pointer, PC_OFFSET);
}

void PPUArm64::bcctr(Instruction instr) {
    writebackPC(recompiler_pc);
    stop_compiling = true;
    FALLBACK(bcctr, instr.raw);
    LDR(scratch1, state_pointer, PC_OFFSET); ADD(scratch1, scratch1, 4); STR(scratch1, state_pointer, PC_OFFSET);
}

#define LOAD2(reg1, reg2, field1, field2)                                      \
  if (instr.field2 == instr.field1 + 1) {                                      \
    LDP(reg1, reg2, state_pointer, GPR_OFFSET(instr.field1));                  \
  } else if (instr.field1 == instr.field2 + 1) {                               \
    LDP(reg2, reg1, state_pointer, GPR_OFFSET(instr.field2));                  \
  } else {                                                                     \
    LDR(reg1, state_pointer, GPR_OFFSET(instr.field1));                        \
    LDR(reg2, state_pointer, GPR_OFFSET(instr.field2));                        \
  }

void PPUArm64::add(Instruction instr) {
    if (!instr.rc) {
        LOAD2(scratch1, scratch2, ra, rb);
        ADD(scratch1, scratch1, scratch2);
        STR(scratch1, state_pointer, GPR_OFFSET(instr.rt));
    }

    else {
        FALLBACK(add, instr.raw);
    }
}

void PPUArm64::and_(Instruction instr) {
    if (!instr.rc) {
        LOAD2(scratch1, scratch2, rs, rb);
        AND(scratch1, scratch1, scratch2);
        STR(scratch1, state_pointer, GPR_OFFSET(instr.ra));
    }

    else {
        FALLBACK(and_, instr.raw); 
    }
}

void PPUArm64::or_(Instruction instr) {
    if (!instr.rc) {
        LOAD2(scratch1, scratch2, rs, rb);
        ORR(scratch1, scratch1, scratch2);
        STR(scratch1, state_pointer, GPR_OFFSET(instr.ra));
    }

    else {
        FALLBACK(or_, instr.raw); 
    }
}

void PPUArm64::xor_(Instruction instr) {
    if (!instr.rc) {
        LOAD2(scratch1, scratch2, rs, rb);
        EOR(scratch1, scratch1, scratch2);
        STR(scratch1, state_pointer, GPR_OFFSET(instr.ra));
    }

    else {
        FALLBACK(xor_, instr.raw); 
    }
}

void PPUArm64::mulli      (Instruction instr) { FALLBACK(mulli, instr.raw); }
void PPUArm64::subfic     (Instruction instr) { FALLBACK(subfic, instr.raw); }
void PPUArm64::cmpli      (Instruction instr) { FALLBACK(cmpli, instr.raw); }
void PPUArm64::cmpi       (Instruction instr) { FALLBACK(cmpi, instr.raw); }
void PPUArm64::addic      (Instruction instr) { FALLBACK(addic, instr.raw); }
void PPUArm64::addic_     (Instruction instr) { FALLBACK(addic_, instr.raw); }
void PPUArm64::addi       (Instruction instr) { FALLBACK(addi, instr.raw); }
void PPUArm64::addis      (Instruction instr) { FALLBACK(addis, instr.raw); }
void PPUArm64::rlwimi     (Instruction instr) { FALLBACK(rlwimi, instr.raw); }
void PPUArm64::rlwinm     (Instruction instr) { FALLBACK(rlwinm, instr.raw); }
void PPUArm64::rlwnm      (Instruction instr) { FALLBACK(rlwnm, instr.raw); }
void PPUArm64::ori        (Instruction instr) { FALLBACK(ori, instr.raw); }
void PPUArm64::oris       (Instruction instr) { FALLBACK(oris, instr.raw); }
void PPUArm64::xori       (Instruction instr) { FALLBACK(xori, instr.raw); }
void PPUArm64::xoris      (Instruction instr) { FALLBACK(xoris, instr.raw); }
void PPUArm64::andi       (Instruction instr) { FALLBACK(andi, instr.raw); }
void PPUArm64::andis      (Instruction instr) { FALLBACK(andis, instr.raw); }
void PPUArm64::lwz        (Instruction instr) { FALLBACK(lwz, instr.raw); }
void PPUArm64::lwzu       (Instruction instr) { FALLBACK(lwzu, instr.raw); }
void PPUArm64::lbz        (Instruction instr) { FALLBACK(lbz, instr.raw); }
void PPUArm64::lbzu       (Instruction instr) { FALLBACK(lbzu, instr.raw); }
void PPUArm64::stw        (Instruction instr) { FALLBACK(stw, instr.raw); }
void PPUArm64::stwu       (Instruction instr) { FALLBACK(stwu, instr.raw); }
void PPUArm64::stb        (Instruction instr) { FALLBACK(stb, instr.raw); }
void PPUArm64::stbu       (Instruction instr) { FALLBACK(stbu, instr.raw); }
void PPUArm64::lhz        (Instruction instr) { FALLBACK(lhz, instr.raw); }
void PPUArm64::lhzu       (Instruction instr) { FALLBACK(lhzu, instr.raw); }
void PPUArm64::sth        (Instruction instr) { FALLBACK(sth, instr.raw); }
void PPUArm64::sthu       (Instruction instr) { FALLBACK(sthu, instr.raw); }
void PPUArm64::lfs        (Instruction instr) { FALLBACK(lfs, instr.raw); }
void PPUArm64::lfsu       (Instruction instr) { FALLBACK(lfsu, instr.raw); }
void PPUArm64::lfd        (Instruction instr) { FALLBACK(lfd, instr.raw); }
void PPUArm64::lfdu       (Instruction instr) { FALLBACK(lfdu, instr.raw); }
void PPUArm64::stfs       (Instruction instr) { FALLBACK(stfs, instr.raw); }
void PPUArm64::stfsu      (Instruction instr) { FALLBACK(stfsu, instr.raw); }
void PPUArm64::stfd       (Instruction instr) { FALLBACK(stfd, instr.raw); }
void PPUArm64::stfdu      (Instruction instr) { FALLBACK(stfdu, instr.raw); }
void PPUArm64::vcmpequb   (Instruction instr) { FALLBACK(vcmpequb, instr.raw); }
void PPUArm64::vaddfp     (Instruction instr) { FALLBACK(vaddfp, instr.raw); }
void PPUArm64::vmhraddshs (Instruction instr) { FALLBACK(vmhraddshs, instr.raw); }
void PPUArm64::vmladduhm  (Instruction instr) { FALLBACK(vmladduhm, instr.raw); }
void PPUArm64::vmsumshm   (Instruction instr) { FALLBACK(vmsumshm, instr.raw); }
void PPUArm64::vsel       (Instruction instr) { FALLBACK(vsel, instr.raw); }
void PPUArm64::vperm      (Instruction instr) { FALLBACK(vperm, instr.raw); }
void PPUArm64::vsldoi     (Instruction instr) { FALLBACK(vsldoi, instr.raw); }
void PPUArm64::vmaddfp    (Instruction instr) { FALLBACK(vmaddfp, instr.raw); }
void PPUArm64::vnmsubfp   (Instruction instr) { FALLBACK(vnmsubfp, instr.raw); }
void PPUArm64::vadduhm    (Instruction instr) { FALLBACK(vadduhm, instr.raw); }
void PPUArm64::vmulouh    (Instruction instr) { FALLBACK(vmulouh, instr.raw); }
void PPUArm64::vsubfp     (Instruction instr) { FALLBACK(vsubfp, instr.raw); }
void PPUArm64::vmrghh     (Instruction instr) { FALLBACK(vmrghh, instr.raw); }
void PPUArm64::vadduwm    (Instruction instr) { FALLBACK(vadduwm, instr.raw); }
void PPUArm64::vrlw       (Instruction instr) { FALLBACK(vrlw, instr.raw); }
void PPUArm64::vcmpequw   (Instruction instr) { FALLBACK(vcmpequw, instr.raw); }
void PPUArm64::vmrghw     (Instruction instr) { FALLBACK(vmrghw, instr.raw); }
void PPUArm64::vcmpeqfp   (Instruction instr) { FALLBACK(vcmpeqfp, instr.raw); }
void PPUArm64::vrefp      (Instruction instr) { FALLBACK(vrefp, instr.raw); }
void PPUArm64::vpkshus    (Instruction instr) { FALLBACK(vpkshus, instr.raw); }
void PPUArm64::vslh       (Instruction instr) { FALLBACK(vslh, instr.raw); }
void PPUArm64::vmulosh    (Instruction instr) { FALLBACK(vmulosh, instr.raw); }
void PPUArm64::vrsqrtefp  (Instruction instr) { FALLBACK(vrsqrtefp, instr.raw); }
void PPUArm64::vmrglh     (Instruction instr) { FALLBACK(vmrglh, instr.raw); }
void PPUArm64::vslw       (Instruction instr) { FALLBACK(vslw, instr.raw); }
void PPUArm64::vmrglw     (Instruction instr) { FALLBACK(vmrglw, instr.raw); }
void PPUArm64::vcmpgefp   (Instruction instr) { FALLBACK(vcmpgefp, instr.raw); }
void PPUArm64::vpkswss    (Instruction instr) { FALLBACK(vpkswss, instr.raw); }
void PPUArm64::vspltb     (Instruction instr) { FALLBACK(vspltb, instr.raw); }
void PPUArm64::vupkhsb    (Instruction instr) { FALLBACK(vupkhsb, instr.raw); }
void PPUArm64::vcmpgtuh   (Instruction instr) { FALLBACK(vcmpgtuh, instr.raw); }
void PPUArm64::vsplth     (Instruction instr) { FALLBACK(vsplth, instr.raw); }
void PPUArm64::vupkhsh    (Instruction instr) { FALLBACK(vupkhsh, instr.raw); }
void PPUArm64::vsrw       (Instruction instr) { FALLBACK(vsrw, instr.raw); }
void PPUArm64::vcmpgtuw   (Instruction instr) { FALLBACK(vcmpgtuw, instr.raw); }
void PPUArm64::vspltw     (Instruction instr) { FALLBACK(vspltw, instr.raw); }
void PPUArm64::vupklsb    (Instruction instr) { FALLBACK(vupklsb, instr.raw); }
void PPUArm64::vcmpgtfp   (Instruction instr) { FALLBACK(vcmpgtfp, instr.raw); }
void PPUArm64::vupklsh    (Instruction instr) { FALLBACK(vupklsh, instr.raw); }
void PPUArm64::vcfux      (Instruction instr) { FALLBACK(vcfux, instr.raw); }
void PPUArm64::vspltisb   (Instruction instr) { FALLBACK(vspltisb, instr.raw); }
void PPUArm64::vaddshs    (Instruction instr) { FALLBACK(vaddshs, instr.raw); }
void PPUArm64::vsrah      (Instruction instr) { FALLBACK(vsrah, instr.raw); }
void PPUArm64::vmulesh    (Instruction instr) { FALLBACK(vmulesh, instr.raw); }
void PPUArm64::vcfsx      (Instruction instr) { FALLBACK(vcfsx, instr.raw); }
void PPUArm64::vspltish   (Instruction instr) { FALLBACK(vspltish, instr.raw); }
void PPUArm64::vsraw      (Instruction instr) { FALLBACK(vsraw, instr.raw); }
void PPUArm64::vcmpgtsw   (Instruction instr) { FALLBACK(vcmpgtsw, instr.raw); }
void PPUArm64::vctuxs     (Instruction instr) { FALLBACK(vctuxs, instr.raw); }
void PPUArm64::vspltisw   (Instruction instr) { FALLBACK(vspltisw, instr.raw); }
void PPUArm64::vctsxs     (Instruction instr) { FALLBACK(vctsxs, instr.raw); }
void PPUArm64::vand       (Instruction instr) { FALLBACK(vand, instr.raw); }
void PPUArm64::vmaxfp     (Instruction instr) { FALLBACK(vmaxfp, instr.raw); }
void PPUArm64::vsubuhm    (Instruction instr) { FALLBACK(vsubuhm, instr.raw); }
void PPUArm64::vandc      (Instruction instr) { FALLBACK(vandc, instr.raw); }
void PPUArm64::vminfp     (Instruction instr) { FALLBACK(vminfp, instr.raw); }
void PPUArm64::vsubuwm    (Instruction instr) { FALLBACK(vsubuwm, instr.raw); }
void PPUArm64::vor        (Instruction instr) { FALLBACK(vor, instr.raw); }
void PPUArm64::vnor       (Instruction instr) { FALLBACK(vnor, instr.raw); }
void PPUArm64::vxor       (Instruction instr) { FALLBACK(vxor, instr.raw); }
void PPUArm64::vsubshs    (Instruction instr) { FALLBACK(vsubshs, instr.raw); }
void PPUArm64::mcrf       (Instruction instr) { FALLBACK(mcrf, instr.raw); }
void PPUArm64::crnor      (Instruction instr) { FALLBACK(crnor, instr.raw); }
void PPUArm64::crandc     (Instruction instr) { FALLBACK(crandc, instr.raw); }
void PPUArm64::crnand     (Instruction instr) { FALLBACK(crnand, instr.raw); }
void PPUArm64::crand      (Instruction instr) { FALLBACK(crand, instr.raw); }
void PPUArm64::crorc      (Instruction instr) { FALLBACK(crorc, instr.raw); }
void PPUArm64::cror       (Instruction instr) { FALLBACK(cror, instr.raw); }
void PPUArm64::rldicl     (Instruction instr) { FALLBACK(rldicl, instr.raw); }
void PPUArm64::rldicr     (Instruction instr) { FALLBACK(rldicr, instr.raw); }
void PPUArm64::rldic      (Instruction instr) { FALLBACK(rldic, instr.raw); }
void PPUArm64::rldimi     (Instruction instr) { FALLBACK(rldimi, instr.raw); }
void PPUArm64::rldcl      (Instruction instr) { FALLBACK(rldcl, instr.raw); }
void PPUArm64::cmp        (Instruction instr) { FALLBACK(cmp, instr.raw); }
void PPUArm64::lvsl       (Instruction instr) { FALLBACK(lvsl, instr.raw); }
void PPUArm64::subfc      (Instruction instr) { FALLBACK(subfc, instr.raw); }
void PPUArm64::mulhdu     (Instruction instr) { FALLBACK(mulhdu, instr.raw); }
void PPUArm64::addc       (Instruction instr) { FALLBACK(addc, instr.raw); }
void PPUArm64::mulhwu     (Instruction instr) { FALLBACK(mulhwu, instr.raw); }
void PPUArm64::mfcr       (Instruction instr) { FALLBACK(mfcr, instr.raw); }
void PPUArm64::lwarx      (Instruction instr) { FALLBACK(lwarx, instr.raw); }
void PPUArm64::ldx        (Instruction instr) { FALLBACK(ldx, instr.raw); }
void PPUArm64::lwzx       (Instruction instr) { FALLBACK(lwzx, instr.raw); }
void PPUArm64::cntlzw     (Instruction instr) { FALLBACK(cntlzw, instr.raw); }
void PPUArm64::slw        (Instruction instr) { FALLBACK(slw, instr.raw); }
void PPUArm64::sld        (Instruction instr) { FALLBACK(sld, instr.raw); }
void PPUArm64::cmpl       (Instruction instr) { FALLBACK(cmpl, instr.raw); }
void PPUArm64::lvsr       (Instruction instr) { FALLBACK(lvsr, instr.raw); }
void PPUArm64::subf       (Instruction instr) { FALLBACK(subf, instr.raw); }
void PPUArm64::lwzux      (Instruction instr) { FALLBACK(lwzux, instr.raw); }
void PPUArm64::cntlzd     (Instruction instr) { FALLBACK(cntlzd, instr.raw); }
void PPUArm64::andc       (Instruction instr) { FALLBACK(andc, instr.raw); }
void PPUArm64::lvewx      (Instruction instr) { FALLBACK(lvewx, instr.raw); }
void PPUArm64::mulhd      (Instruction instr) { FALLBACK(mulhd, instr.raw); }
void PPUArm64::mulhw      (Instruction instr) { FALLBACK(mulhw, instr.raw); }
void PPUArm64::ldarx      (Instruction instr) { FALLBACK(ldarx, instr.raw); }
void PPUArm64::lbzx       (Instruction instr) { FALLBACK(lbzx, instr.raw); }
void PPUArm64::lvx        (Instruction instr) { FALLBACK(lvx, instr.raw); }
void PPUArm64::neg        (Instruction instr) { FALLBACK(neg, instr.raw); }
void PPUArm64::nor        (Instruction instr) { FALLBACK(nor, instr.raw); }
void PPUArm64::stvebx     (Instruction instr) { FALLBACK(stvebx, instr.raw); }
void PPUArm64::subfe      (Instruction instr) { FALLBACK(subfe, instr.raw); }
void PPUArm64::adde       (Instruction instr) { FALLBACK(adde, instr.raw); }
void PPUArm64::mtcrf      (Instruction instr) { FALLBACK(mtcrf, instr.raw); }
void PPUArm64::stdx       (Instruction instr) { FALLBACK(stdx, instr.raw); }
void PPUArm64::stwcx      (Instruction instr) { FALLBACK(stwcx, instr.raw); }
void PPUArm64::stwx       (Instruction instr) { FALLBACK(stwx, instr.raw); }
void PPUArm64::stvehx     (Instruction instr) { FALLBACK(stvehx, instr.raw); }
void PPUArm64::stdux      (Instruction instr) { FALLBACK(stdux, instr.raw); }
void PPUArm64::stvewx     (Instruction instr) { FALLBACK(stvewx, instr.raw); }
void PPUArm64::addze      (Instruction instr) { FALLBACK(addze, instr.raw); }
void PPUArm64::stdcx      (Instruction instr) { FALLBACK(stdcx, instr.raw); }
void PPUArm64::stbx       (Instruction instr) { FALLBACK(stbx, instr.raw); }
void PPUArm64::stvx       (Instruction instr) { FALLBACK(stvx, instr.raw); }
void PPUArm64::mulld      (Instruction instr) { FALLBACK(mulld, instr.raw); }
void PPUArm64::mullw      (Instruction instr) { FALLBACK(mullw, instr.raw); }
void PPUArm64::stbux      (Instruction instr) { FALLBACK(stbux, instr.raw); }
void PPUArm64::lhzx       (Instruction instr) { FALLBACK(lhzx, instr.raw); }
void PPUArm64::mfspr      (Instruction instr) { FALLBACK(mfspr, instr.raw); }
void PPUArm64::mftb       (Instruction instr) { FALLBACK(mftb, instr.raw); }
void PPUArm64::sthx       (Instruction instr) { FALLBACK(sthx, instr.raw); }
void PPUArm64::orc        (Instruction instr) { FALLBACK(orc, instr.raw); }
void PPUArm64::divdu      (Instruction instr) { FALLBACK(divdu, instr.raw); }
void PPUArm64::divwu      (Instruction instr) { FALLBACK(divwu, instr.raw); }
void PPUArm64::mtspr      (Instruction instr) { FALLBACK(mtspr, instr.raw); }
void PPUArm64::nand       (Instruction instr) { FALLBACK(nand, instr.raw); }
void PPUArm64::divd       (Instruction instr) { FALLBACK(divd, instr.raw); }
void PPUArm64::divw       (Instruction instr) { FALLBACK(divw, instr.raw); }
void PPUArm64::lvlx       (Instruction instr) { FALLBACK(lvlx, instr.raw); }
void PPUArm64::lwbrx      (Instruction instr) { FALLBACK(lwbrx, instr.raw); }
void PPUArm64::lfsx       (Instruction instr) { FALLBACK(lfsx, instr.raw); }
void PPUArm64::srw        (Instruction instr) { FALLBACK(srw, instr.raw); }
void PPUArm64::srd        (Instruction instr) { FALLBACK(srd, instr.raw); }
void PPUArm64::lvrx       (Instruction instr) { FALLBACK(lvrx, instr.raw); }
void PPUArm64::lfdx       (Instruction instr) { FALLBACK(lfdx, instr.raw); }
void PPUArm64::stvlx      (Instruction instr) { FALLBACK(stvlx, instr.raw); }
void PPUArm64::stfsx      (Instruction instr) { FALLBACK(stfsx, instr.raw); }
void PPUArm64::stvrx      (Instruction instr) { FALLBACK(stvrx, instr.raw); }
void PPUArm64::stfdx      (Instruction instr) { FALLBACK(stfdx, instr.raw); }
void PPUArm64::lhbrx      (Instruction instr) { FALLBACK(lhbrx, instr.raw); }
void PPUArm64::sraw       (Instruction instr) { FALLBACK(sraw, instr.raw); }
void PPUArm64::srad       (Instruction instr) { FALLBACK(srad, instr.raw); }
void PPUArm64::srawi      (Instruction instr) { FALLBACK(srawi, instr.raw); }
void PPUArm64::sradi      (Instruction instr) { FALLBACK(sradi, instr.raw); }
void PPUArm64::extsh      (Instruction instr) { FALLBACK(extsh, instr.raw); }
void PPUArm64::extsb      (Instruction instr) { FALLBACK(extsb, instr.raw); }
void PPUArm64::extsw      (Instruction instr) { FALLBACK(extsw, instr.raw); }
void PPUArm64::stfiwx     (Instruction instr) { FALLBACK(stfiwx, instr.raw); }
void PPUArm64::dcbz       (Instruction instr) { FALLBACK(dcbz, instr.raw); }
void PPUArm64::ld         (Instruction instr) { FALLBACK(ld, instr.raw); }
void PPUArm64::ldu        (Instruction instr) { FALLBACK(ldu, instr.raw); }
void PPUArm64::lwa        (Instruction instr) { FALLBACK(lwa, instr.raw); }
void PPUArm64::fdivs      (Instruction instr) { FALLBACK(fdivs, instr.raw); }
void PPUArm64::fsubs      (Instruction instr) { FALLBACK(fsubs, instr.raw); }
void PPUArm64::fadds      (Instruction instr) { FALLBACK(fadds, instr.raw); }
void PPUArm64::fsqrts     (Instruction instr) { FALLBACK(fsqrts, instr.raw); }
void PPUArm64::fmuls      (Instruction instr) { FALLBACK(fmuls, instr.raw); }
void PPUArm64::fmsubs     (Instruction instr) { FALLBACK(fmsubs, instr.raw); }
void PPUArm64::fmadds     (Instruction instr) { FALLBACK(fmadds, instr.raw); }
void PPUArm64::fnmsubs    (Instruction instr) { FALLBACK(fnmsubs, instr.raw); }
void PPUArm64::fnmadds    (Instruction instr) { FALLBACK(fnmadds, instr.raw); }
void PPUArm64::std        (Instruction instr) { FALLBACK(std, instr.raw); }
void PPUArm64::stdu       (Instruction instr) { FALLBACK(stdu, instr.raw); }
void PPUArm64::mffs       (Instruction instr) { FALLBACK(mffs, instr.raw); }
void PPUArm64::mtfsf      (Instruction instr) { FALLBACK(mtfsf, instr.raw); }
void PPUArm64::fcmpu      (Instruction instr) { FALLBACK(fcmpu, instr.raw); }
void PPUArm64::frsp       (Instruction instr) { FALLBACK(frsp, instr.raw); }
void PPUArm64::fctiwz     (Instruction instr) { FALLBACK(fctiwz, instr.raw); }
void PPUArm64::fdiv       (Instruction instr) { FALLBACK(fdiv, instr.raw); }
void PPUArm64::fsub       (Instruction instr) { FALLBACK(fsub, instr.raw); }
void PPUArm64::fadd       (Instruction instr) { FALLBACK(fadd, instr.raw); }
void PPUArm64::fsqrt      (Instruction instr) { FALLBACK(fsqrt, instr.raw); }
void PPUArm64::fsel       (Instruction instr) { FALLBACK(fsel, instr.raw); }
void PPUArm64::fmul       (Instruction instr) { FALLBACK(fmul, instr.raw); }
void PPUArm64::frsqrte    (Instruction instr) { FALLBACK(frsqrte, instr.raw); }
void PPUArm64::fmr        (Instruction instr) { FALLBACK(fmr, instr.raw); }
void PPUArm64::fmsub      (Instruction instr) { FALLBACK(fmsub, instr.raw); }
void PPUArm64::fmadd      (Instruction instr) { FALLBACK(fmadd, instr.raw); }
void PPUArm64::fnmsub     (Instruction instr) { FALLBACK(fnmsub, instr.raw); }
void PPUArm64::fnmadd     (Instruction instr) { FALLBACK(fnmadd, instr.raw); }
void PPUArm64::fneg       (Instruction instr) { FALLBACK(fneg, instr.raw); }
void PPUArm64::fabs_      (Instruction instr) { FALLBACK(fabs_, instr.raw); }
void PPUArm64::fctid      (Instruction instr) { FALLBACK(fctid, instr.raw); }
void PPUArm64::fctidz     (Instruction instr) { FALLBACK(fctidz, instr.raw); }
void PPUArm64::fcfid      (Instruction instr) { FALLBACK(fcfid, instr.raw); }

void PPUArm64::compileInstruction (Instruction instr) {
    switch (instr.opc) {
        case G_04: {
            switch (instr.g_04_field & 0x3f) {
                case VMADDFP:       vmaddfp(instr);     break;
                case VMHRADDSHS:    vmhraddshs(instr);  break;
                case VMLADDUHM:     vmladduhm(instr);   break;
                case VNMSUBFP:      vnmsubfp(instr);    break;
                case VMSUMSHM:      vmsumshm(instr);    break;
                case VSEL:          vsel(instr);        break;
                case VPERM:         vperm(instr);       break;
                case VSLDOI:        vsldoi(instr);      break;
                    
                default:
                    switch (instr.g_04_field) {
                            
                        case VCMPEQUB:  vcmpequb(instr);    break;
                        case VADDFP:    vaddfp(instr);      break;
                        case VADDUHM:   vadduhm(instr);     break;
                        case VMULOUH:   vmulouh(instr);     break;
                        case VSUBFP:    vsubfp(instr);      break;
                        case VMRGHH:    vmrghh(instr);      break;
                        case VADDUWM:   vadduwm(instr);     break;
                        case VRLW:      vrlw(instr);        break;
                        case VCMPEQUW_:
                        case VCMPEQUW:  vcmpequw(instr);    break;
                        case VMRGHW:    vmrghw(instr);      break;
                        case VCMPEQFP_:
                        case VCMPEQFP:  vcmpeqfp(instr);    break;
                        case VREFP:     vrefp(instr);       break;
                        case VPKSHUS:   vpkshus(instr);     break;
                        case VSLH:      vslh(instr);        break;
                        case VMULOSH:   vmulosh(instr);     break;
                        case VRSQRTEFP: vrsqrtefp(instr);   break;
                        case VMRGLH:    vmrglh(instr);      break;
                        case VSLW:      vslw(instr);        break;
                        case VMRGLW:    vmrglw(instr);      break;
                        case VCMPGEFP_:
                        case VCMPGEFP:  vcmpgefp(instr);    break;
                        case VPKSWSS:   vpkswss(instr);     break;
                        case VSPLTB:    vspltb(instr);      break;
                        case VUPKHSB:   vupkhsb(instr);     break;
                        case VCMPGTUH_:
                        case VCMPGTUH:  vcmpgtuh(instr);    break;
                        case VSPLTH:    vsplth(instr);      break;
                        case VUPKHSH:   vupkhsh(instr);     break;
                        case VSRW:      vsrw(instr);        break;
                        case VCMPGTUW_:
                        case VCMPGTUW:  vcmpgtuw(instr);    break;
                        case VSPLTW:    vspltw(instr);      break;
                        case VUPKLSB:   vupklsb(instr);     break;
                        case VCMPGTFP_:
                        case VCMPGTFP:  vcmpgtfp(instr);    break;
                        case VUPKLSH:   vupklsh(instr);     break;
                        case VCFUX:     vcfux(instr);       break;
                        case VSPLTISB:  vspltisb(instr);    break;
                        case VADDSHS:   vaddshs(instr);     break;
                        case VSRAH:     vsrah(instr);       break;
                        case VMULESH:   vmulesh(instr);     break;
                        case VCFSX:     vcfsx(instr);       break;
                        case VSPLTISH:  vspltish(instr);    break;
                        case VSRAW:     vsraw(instr);       break;
                        case VCMPGTSW:  vcmpgtsw(instr);    break;
                        case VCTUXS:    vctuxs(instr);      break;
                        case VSPLTISW:  vspltisw(instr);    break;
                        case VCTSXS:    vctsxs(instr);      break;
                        case VAND:      vand(instr);        break;
                        case VMAXFP:    vmaxfp(instr);      break;
                        case VSUBUHM:   vsubuhm(instr);     break;
                        case VANDC:     vandc(instr);       break;
                        case VMINFP:    vminfp(instr);      break;
                        case VSUBUWM:   vsubuwm(instr);     break;
                        case VOR:       vor(instr);         break;
                        case VNOR:      vnor(instr);        break;
                        case VXOR:      vxor(instr);        break;
                        case VSUBSHS:   vsubshs(instr);     break;
                            
                        default:
                            Helpers::panic("Unimplemented G_04 instruction 0x%02x (decimal: %d) (full instr: 0x%08x) @ 0x%016llx\n", (u32)instr.g_04_field, (u32)instr.g_04_field, instr.raw, recompiler_pc);
                    }
            }
            break;
        }
        case Instructions::MULLI:  mulli(instr);   break;
        case Instructions::SUBFIC: subfic(instr);  break;
        case Instructions::CMPLI:  cmpli(instr);   break;
        case Instructions::CMPI:   cmpi(instr);    break;
        case Instructions::ADDIC:  addic(instr);   break;
        case Instructions::ADDIC_: addic_(instr);  break;
        case Instructions::ADDI:   addi(instr);    break;
        case Instructions::ADDIS:  addis(instr);   break;
        case Instructions::BC:     bc(instr);      break;
        case Instructions::SC:     sc(instr);      break;
        case Instructions::B:      b(instr);       break;

        case G_13: {
            switch (instr.g_13_field) {
                    
                case MCRF:      mcrf(instr);    break;
                case BCLR:      bclr(instr);    break;
                case CRNOR:     crnor(instr);   break;
                case CRANDC:    crandc(instr);  break;
                case ISYNC:     break;
                case CRNAND:    crnand(instr);  break;
                case CRAND:     crand(instr);  break;
                case CRORC:     crorc(instr);   break;
                case CROR:      cror(instr);    break;
                case BCCTR:     bcctr(instr);   break;
                    
                default:
                    Helpers::panic("Unimplemented G_13 instruction 0x%02x (decimal: %d) (full instr: 0x%08x) @ 0x%016llx\n", (u32)instr.g_13_field, (u32)instr.g_13_field, instr.raw, recompiler_pc);
            }
            break;
        }
        case RLWIMI:    rlwimi(instr);  break;
        case RLWINM:    rlwinm(instr);  break;
        case RLWNM:     rlwnm(instr);   break;
        case ORI:       ori(instr);     break;
        case ORIS:      oris(instr);    break;
        case XORI:      xori(instr);    break;
        case XORIS:     xoris(instr);   break;
        case ANDI:      andi(instr);    break;
        case ANDIS:     andis(instr);   break;
        case G_1E: {
            switch (instr.g_1e_field) {
                    
                case RLDICL_:
                case RLDICL:    rldicl(instr);  break;
                case RLDICR_:
                case RLDICR:    rldicr(instr);  break;
                case RLDIC_:
                case RLDIC:     rldic(instr);   break;
                case RLDIMI_:
                case RLDIMI:    rldimi(instr);  break;
                case RLDCL:     rldcl(instr);   break;
                    
                default:
                    Helpers::panic("Unimplemented G_1E instruction 0x%02x (decimal: %d) (full instr: 0x%08x)\n", (u32)instr.g_1e_field, (u32)instr.g_1e_field, instr.raw);
            }
            break;
        }
        case G_1F: {
            switch (instr.g_1f_field) {
                case G_1FOpcodes::CMP:       cmp(instr);     break;
                //case G_1FOpcodes::TW:        ps3->thread_manager.getCurrentThread()->wait(); break;
                case G_1FOpcodes::LVSL:      lvsl(instr);    break;
                case G_1FOpcodes::SUBFC:     subfc(instr);   break;
                case G_1FOpcodes::MULHDU:    mulhdu(instr);  break;
                case G_1FOpcodes::ADDC:      addc(instr);    break;
                case G_1FOpcodes::MULHWU:    mulhwu(instr);  break;
                case G_1FOpcodes::MFCR:      mfcr(instr);    break;
                case G_1FOpcodes::LWARX:     lwarx(instr);   break;
                case G_1FOpcodes::LDX:       ldx(instr);     break;
                case G_1FOpcodes::LWZX:      lwzx(instr);    break;
                case G_1FOpcodes::CNTLZW:    cntlzw(instr);  break;
                case G_1FOpcodes::SLW:       slw(instr);     break;
                case G_1FOpcodes::SLD:       sld(instr);     break;
                case G_1FOpcodes::AND:       and_(instr);    break;
                case G_1FOpcodes::CMPL:      cmpl(instr);    break;
                case G_1FOpcodes::LVSR:      lvsr(instr);    break;
                case G_1FOpcodes::SUBF:      subf(instr);    break;
                case G_1FOpcodes::DCBST:     break;
                case G_1FOpcodes::LWZUX:     lwzux(instr);   break;
                case G_1FOpcodes::CNTLZD:    cntlzd(instr);  break;
                case G_1FOpcodes::ANDC:      andc(instr);    break;
                case G_1FOpcodes::LVEWX:     lvewx(instr);   break;
                case G_1FOpcodes::MULHD:     mulhd(instr);   break;
                case G_1FOpcodes::MULHW:     mulhw(instr);   break;
                case G_1FOpcodes::LDARX:     ldarx(instr);   break;
                case G_1FOpcodes::LBZX:      lbzx(instr);    break;
                case G_1FOpcodes::LVX:       lvx(instr);     break;
                case G_1FOpcodes::NEG:       neg(instr);     break;
                case G_1FOpcodes::NOR:       nor(instr);     break;
                case G_1FOpcodes::STVEBX:    stvebx(instr);  break;
                case G_1FOpcodes::SUBFE:     subfe(instr);   break;
                case G_1FOpcodes::ADDE:      adde(instr);    break;
                case G_1FOpcodes::MTCRF:     mtcrf(instr);   break;
                case G_1FOpcodes::STDX:      stdx(instr);    break;
                case G_1FOpcodes::STWCX_:    stwcx(instr);   break;
                case G_1FOpcodes::STWX:      stwx(instr);    break;
                case G_1FOpcodes::STVEHX:    stvehx(instr);  break;
                case G_1FOpcodes::STDUX:     stdux(instr);   break;
                case G_1FOpcodes::STVEWX:    stvewx(instr);  break;
                case G_1FOpcodes::ADDZE:     addze(instr);   break;
                case G_1FOpcodes::STDCX_:    stdcx(instr);   break;
                case G_1FOpcodes::STBX:      stbx(instr);    break;
                case G_1FOpcodes::STVX:      stvx(instr);    break;
                case G_1FOpcodes::MULLD:     mulld(instr);   break;
                case G_1FOpcodes::MULLW:     mullw(instr);   break;
                case G_1FOpcodes::STBUX:     stbux(instr);   break;
                case G_1FOpcodes::DCBTST:    break;
                case G_1FOpcodes::ADD:       add(instr);     break;
                case G_1FOpcodes::DCBT:      break;
                case G_1FOpcodes::LHZX:      lhzx(instr);    break;
                case G_1FOpcodes::XOR:       xor_(instr);    break;
                case G_1FOpcodes::MFSPR:     mfspr(instr);   break;
                case G_1FOpcodes::DST:       break;
                case G_1FOpcodes::MFTB:      mftb(instr);    break;
                case G_1FOpcodes::DSTST:     break;
                case G_1FOpcodes::STHX:      sthx(instr);    break;
                case G_1FOpcodes::ORC:       orc(instr);     break;
                case G_1FOpcodes::OR:        or_(instr);     break;
                case G_1FOpcodes::DIVDU:     divdu(instr);   break;
                case G_1FOpcodes::DIVWU:     divwu(instr);   break;
                case G_1FOpcodes::MTSPR:     mtspr(instr);   break;
                case G_1FOpcodes::NAND:      nand(instr);    break;
                case G_1FOpcodes::DIVD:      divd(instr);    break;
                case G_1FOpcodes::DIVW:      divw(instr);    break;
                case G_1FOpcodes::LVLX:      lvlx(instr);    break;
                case G_1FOpcodes::LWBRX:     lwbrx(instr);   break;
                case G_1FOpcodes::LFSX:      lfsx(instr);    break;
                case G_1FOpcodes::SRW:       srw(instr);     break;
                case G_1FOpcodes::SRD:       srd(instr);     break;
                case G_1FOpcodes::LVRX:      lvrx(instr);    break;
                case G_1FOpcodes::SYNC:      break;
                case G_1FOpcodes::LFDX:      lfdx(instr);    break;
                case G_1FOpcodes::STVLX:     stvlx(instr);   break;
                case G_1FOpcodes::STFSX:     stfsx(instr);   break;
                case G_1FOpcodes::STVRX:     stvrx(instr);   break;
                case G_1FOpcodes::STFDX:     stfdx(instr);   break;
                case G_1FOpcodes::LHBRX:     lhbrx(instr);   break;
                case G_1FOpcodes::SRAW:      sraw(instr);    break;
                case G_1FOpcodes::SRAD:      srad(instr);    break;
                case G_1FOpcodes::DSS:       break;
                case G_1FOpcodes::SRAWI:     srawi(instr);   break;
                case G_1FOpcodes::SRADI1:
                case G_1FOpcodes::SRADI2:    sradi(instr);   break;
                case G_1FOpcodes::EIEIO:     break;
                case G_1FOpcodes::EXTSH:     extsh(instr);   break;
                case G_1FOpcodes::EXTSB:     extsb(instr);   break;
                case G_1FOpcodes::EXTSW:     extsw(instr);   break;
                case G_1FOpcodes::STFIWX:    stfiwx(instr);  break;
                case G_1FOpcodes::DCBZ:      dcbz(instr);    break;
                    
                default:
                    Helpers::panic("Unimplemented G_1F instruction 0x%03x (decimal: %d) (full instr: 0x%08x) @ 0x%016llx\n", (u32)instr.g_1f_field, (u32)instr.g_1f_field, instr.raw, recompiler_pc);
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
        case LHZU:  lhzu(instr);    break;
        case STH:   sth(instr);     break;
        case STHU:  sthu(instr);    break;
        case LFS:   lfs(instr);     break;
        case LFSU:  lfsu(instr);     break;
        case LFD:   lfd(instr);     break;
        case LFDU:  lfdu(instr);    break;
        case STFS:  stfs(instr);    break;
        case STFSU: stfsu(instr);   break;
        case STFD:  stfd(instr);    break;
        case STFDU: stfdu(instr);   break;
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
                case FSQRTS:    fsqrts(instr);  break;
                case FMULS:     fmuls(instr);   break;
                case FMSUBS:    fmsubs(instr);  break;
                case FMADDS:    fmadds(instr);  break;
                case FNMSUBS:   fnmsubs(instr); break;
                case FNMADDS:   fnmadds(instr); break;
                    
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
                    
                case G_3FOpcodes::FSEL:      fsel(instr);    break;
                case G_3FOpcodes::FMUL:      fmul(instr);    break;
                case G_3FOpcodes::FRSQRTE:   frsqrte(instr); break;
                case G_3FOpcodes::FMSUB:     fmsub(instr);   break;
                case G_3FOpcodes::FMADD:     fmadd(instr);   break;
                case G_3FOpcodes::FNMSUB:    fnmsub(instr);  break;
                case G_3FOpcodes::FNMADD:    fnmadd(instr);  break;
                    
                default:
                    switch (instr.g_3f_field) {
                            
                        case G_3FOpcodes::MFFS:      mffs(instr);    break;
                        case G_3FOpcodes::MTFSF:     mtfsf(instr);   break;
                        case G_3FOpcodes::FCMPU:     fcmpu(instr);   break;
                        case G_3FOpcodes::FRSP:      frsp(instr);    break;
                        case G_3FOpcodes::FCTIW:
                        case G_3FOpcodes::FCTIWZ:    fctiwz(instr);  break;
                        case G_3FOpcodes::FDIV:      fdiv(instr);    break;
                        case G_3FOpcodes::FSUB:      fsub(instr);    break;
                        case G_3FOpcodes::FADD:      fadd(instr);    break;
                        case G_3FOpcodes::FSQRT:     fsqrt(instr);   break;
                        case G_3FOpcodes::FMR:       fmr(instr);     break;
                        case G_3FOpcodes::FNEG:      fneg(instr);    break;
                        case G_3FOpcodes::FABS:      fabs_(instr);   break;
                        case G_3FOpcodes::FCTID:     fctid(instr);   break;
                        case G_3FOpcodes::FCTIDZ:    fctidz(instr);  break;
                        case G_3FOpcodes::FCFID:     fcfid(instr);   break;
                            
                        default:
                            Helpers::panic("Unimplemented G_3F instruction 0x%02x (decimal: %d) (full instr: 0x%08x) @ 0x%016llx\n", (u32)instr.g_3f_field, (u32)instr.g_3f_field, instr.raw, recompiler_pc);
                    }
            }
            break;
        }
            
        default:
            Helpers::panic("Unimplemented opcode 0x%02x (decimal: %d) (full instr: 0x%08x) @ 0x%016llx\n", (u32)instr.opc, (u32)instr.opc, instr.raw, recompiler_pc);
    }
}