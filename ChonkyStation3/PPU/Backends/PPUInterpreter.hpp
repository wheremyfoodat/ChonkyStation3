#pragma once

#include <PPU.hpp>

// Circular dependency
class PlayStation3;

using namespace PPUTypes;

class PPUInterpreter : public PPU {
public:
    PPUInterpreter(Memory& mem, PlayStation3* ps3) : PPU(mem, ps3) {}
    void step() override;

    // Debug symbols
    void printFunctionCall();

    // Main
    void mulli      (const Instruction& instr);
    void subfic     (const Instruction& instr);
    void cmpli      (const Instruction& instr);
    void cmpi       (const Instruction& instr);
    void addi       (const Instruction& instr);
    void addis      (const Instruction& instr);
    void bc         (const Instruction& instr);
    void sc         (const Instruction& instr);
    void b          (const Instruction& instr);
    void rlwinm     (const Instruction& instr);
    void ori        (const Instruction& instr);
    void oris       (const Instruction& instr);
    void xori       (const Instruction& instr);
    void xoris      (const Instruction& instr);
    void andi       (const Instruction& instr);
    void andis      (const Instruction& instr);
    void lwz        (const Instruction& instr);
    void lwzu       (const Instruction& instr);
    void lbz        (const Instruction& instr);
    void lbzu       (const Instruction& instr);
    void stw        (const Instruction& instr);
    void stwu       (const Instruction& instr);
    void stb        (const Instruction& instr);
    void stbu       (const Instruction& instr);
    void lhz        (const Instruction& instr);
    void sth        (const Instruction& instr);
    void lfs        (const Instruction& instr);
    void lfd        (const Instruction& instr);
    void stfs       (const Instruction& instr);
    void stfd       (const Instruction& instr);
    // G_04
    void vcmpequb   (const Instruction& instr);
    void vaddfp     (const Instruction& instr);
    void vsel       (const Instruction& instr);
    void vperm      (const Instruction& instr);
    void vsldoi     (const Instruction& instr);
    void vmaddfp    (const Instruction& instr);
    void vnmsubfp   (const Instruction& instr);
    void vsubfp     (const Instruction& instr);
    void vadduwm    (const Instruction& instr);
    void vcmpequw   (const Instruction& instr);
    void vmrghw     (const Instruction& instr);
    void vrefp      (const Instruction& instr);
    void vrsqrtefp  (const Instruction& instr);
    void vslw       (const Instruction& instr);
    void vmrglw     (const Instruction& instr);
    void vspltw     (const Instruction& instr);
    void vcfsx      (const Instruction& instr);
    void vspltisw   (const Instruction& instr);
    void vctsxs     (const Instruction& instr);
    void vand       (const Instruction& instr);
    void vor        (const Instruction& instr);
    void vxor       (const Instruction& instr);
    // G_13
    void bclr       (const Instruction& instr);
    void bcctr      (const Instruction& instr);
    // G_1E
    void rldicl     (const Instruction& instr);
    void rldicr     (const Instruction& instr);
    void rldic      (const Instruction& instr);
    void rldimi     (const Instruction& instr);
    // G_1F
    void cmp        (const Instruction& instr);
    void lvsl       (const Instruction& instr);
    void mulhdu     (const Instruction& instr);
    void mfcr       (const Instruction& instr);
    void ldx        (const Instruction& instr);
    void lwzx       (const Instruction& instr);
    void cntlzw     (const Instruction& instr);
    void sld        (const Instruction& instr);
    void and_       (const Instruction& instr);
    void cmpl       (const Instruction& instr);
    void lvsr       (const Instruction& instr);
    void subf       (const Instruction& instr);
    void cntlzd     (const Instruction& instr);
    void andc       (const Instruction& instr);
    void lbzx       (const Instruction& instr);
    void lvx        (const Instruction& instr);
    void neg        (const Instruction& instr);
    void nor        (const Instruction& instr);
    void mtcrf      (const Instruction& instr);
    void stdx       (const Instruction& instr);
    void addze      (const Instruction& instr);
    void stvx       (const Instruction& instr);
    void mulld      (const Instruction& instr);
    void mullw      (const Instruction& instr);
    void add        (const Instruction& instr);
    void xor_       (const Instruction& instr);
    void mfspr      (const Instruction& instr);
    void or_        (const Instruction& instr);
    void divdu      (const Instruction& instr);
    void divwu      (const Instruction& instr);
    void mtspr      (const Instruction& instr);
    void srw        (const Instruction& instr);
    void srawi      (const Instruction& instr);
    void sradi      (const Instruction& instr);
    void extsh      (const Instruction& instr);
    void extsb      (const Instruction& instr);
    void extsw      (const Instruction& instr);
    void stfiwx     (const Instruction& instr);
    // G_3A
    void ld         (const Instruction& instr);
    void ldu        (const Instruction& instr);
    // G_3B
    void fdivs      (const Instruction& instr);
    void fsubs      (const Instruction& instr);
    void fadds      (const Instruction& instr);
    void fmuls      (const Instruction& instr);
    void fmadds     (const Instruction& instr);
    // G_3E
    void std        (const Instruction& instr);
    void stdu       (const Instruction& instr);
    // G_3F
    void fcmpu      (const Instruction& instr);
    void frsp       (const Instruction& instr);
    void fctiwz     (const Instruction& instr);
    void fadd       (const Instruction& instr);
    void fmr        (const Instruction& instr);
    void fmadd      (const Instruction& instr);
    void fneg       (const Instruction& instr);
    void fcfid      (const Instruction& instr);
};