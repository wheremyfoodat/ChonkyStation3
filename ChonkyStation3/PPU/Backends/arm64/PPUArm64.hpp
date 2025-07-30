#pragma once

#include <PPU.hpp>
#include <PPU/Backends/PPUInterpreter.hpp>

#include <oaknut/code_block.hpp>
#include <oaknut/oaknut.hpp>

#include <limits>

// Circular dependency
class PlayStation3;

using namespace PPUTypes;
using namespace oaknut;
using namespace oaknut::util;

class PPUArm64 : public PPU, private oaknut::CodeBlock, public oaknut::CodeGenerator {
    // Interpreter for instruction fallbacks
    PPUInterpreter interpreter;

    static constexpr XReg arg1 = X0;
    static constexpr XReg arg2 = X1;
    static constexpr XReg scratch1 = X9;
    static constexpr XReg scratch2 = X10;
    static constexpr XReg state_pointer = X15;

    static constexpr size_t executable_memory_size = 128_MB;
    // Allocate some extra space as padding for security purposes in the extremely unlikely occasion we manage to overflow the above size
    static constexpr size_t alloc_size = executable_memory_size + 0x10000;

    static constexpr u32 page_shift = 12;
    static constexpr u32 page_size = 1 << page_shift;
    static constexpr u32 page_mask = page_size - 1;

    // A function pointer to JIT-emitted code
    // JIT code returns the number of cycles executed, and takes a pointer to the JIT object in arg0
    using JITCallback = u32 (*)(PPUArm64* ppu);
    
    JITCallback** code_blocks; // First level of 2-level lookup for code
    JITCallback dispatcher;    // Assembly dispatcher

    u32 recompiler_pc;

    void emitDispatcher();
    void emitBlockLookup();
    JITCallback compileBlock();

    template <typename T>
    T getLabelPointer(const oaknut::Label& label) {
        auto pointer = reinterpret_cast<u8*>(oaknut::CodeBlock::ptr()) + label.offset();
        return reinterpret_cast<T>(pointer);
    }

    static u32 runInterpreterThunk(PPUArm64* ppu) { return ppu->interpreter.step(); }

    // Calculate the number of instructions between the current PC and the branch target
    // Returns a negative number for backwards jumps
    int64_t getPCOffset(const void* current, const void* target) {
        return (int64_t)((ptrdiff_t)target - (ptrdiff_t)current) >> 2;
    }

    // Checks if a branch displacement can fit in a 26-bit int, to emit a direct call/jump instead of an indirect one
    bool isInt26(int64_t disp) {
        return disp >= -(1ll << 25) && disp <= ((1ll << 25) - 1);
    }

    // Prepare for a call to a C++ function and then actually emit it
    // Can use and thrash a register if the function we're calling is too far away and needs an indirect call
    template <typename T>
    void call(T func, XReg scratch) {
        const auto ptr = reinterpret_cast<const void*>(func);
        const int64_t disp = getPCOffset(xptr<const void*>(), ptr);

        // If the displacement can fit in a 26-bit int, that means we can emit a direct call to the address
        // Otherwise, load the address into a register and emit a blr
        const bool canDoDirectCall = isInt26(disp);

        if (canDoDirectCall) {
            BL(disp);
        } else {
            MOV(scratch, (uintptr_t)ptr);
            BLR(scratch);
        }
    }

    // Same as above, but jumps instead of calling
    template <typename T>
    void jump(T func, XReg scratch) {
        const auto ptr = reinterpret_cast<const void*>(func);
        const int64_t disp = getPCOffset(xptr<const void*>(), ptr);

        // If the displacement can fit in a 26-bit int, that means we can emit a direct call to the address
        // Otherwise, load the address into a register and emit a blr
        const bool canDoDirectJump = isInt26(disp) && false;

        if (canDoDirectJump) {
            B(disp);
        } else {
            MOV(scratch, (uintptr_t)ptr);
            BR(scratch);
        }
    }


public:
    PPUArm64(Memory& mem, PlayStation3* ps3);
    int step() override;
    bool should_break = false;
    
    // Main
    void mulli      (const Instruction& instr);
    void subfic     (const Instruction& instr);
    void cmpli      (const Instruction& instr);
    void cmpi       (const Instruction& instr);
    void addic      (const Instruction& instr);
    void addic_     (const Instruction& instr);
    void addi       (const Instruction& instr);
    void addis      (const Instruction& instr);
    void bc         (const Instruction& instr);
    void sc         (const Instruction& instr);
    void b          (const Instruction& instr);
    void rlwimi     (const Instruction& instr);
    void rlwinm     (const Instruction& instr);
    void rlwnm      (const Instruction& instr);
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
    void lhzu       (const Instruction& instr);
    void sth        (const Instruction& instr);
    void sthu       (const Instruction& instr);
    void lfs        (const Instruction& instr);
    void lfsu       (const Instruction& instr);
    void lfd        (const Instruction& instr);
    void lfdu       (const Instruction& instr);
    void stfs       (const Instruction& instr);
    void stfsu      (const Instruction& instr);
    void stfd       (const Instruction& instr);
    void stfdu      (const Instruction& instr);
    // G_04
    void vcmpequb   (const Instruction& instr);
    void vaddfp     (const Instruction& instr);
    void vmhraddshs (const Instruction& instr);
    void vmladduhm  (const Instruction& instr);
    void vmsumshm   (const Instruction& instr);
    void vsel       (const Instruction& instr);
    void vperm      (const Instruction& instr);
    void vsldoi     (const Instruction& instr);
    void vmaddfp    (const Instruction& instr);
    void vnmsubfp   (const Instruction& instr);
    void vadduhm    (const Instruction& instr);
    void vmulouh    (const Instruction& instr);
    void vsubfp     (const Instruction& instr);
    void vmrghh     (const Instruction& instr);
    void vadduwm    (const Instruction& instr);
    void vrlw       (const Instruction& instr);
    void vcmpequw   (const Instruction& instr);
    void vmrghw     (const Instruction& instr);
    void vcmpeqfp   (const Instruction& instr);
    void vrefp      (const Instruction& instr);
    void vpkshus    (const Instruction& instr);
    void vslh       (const Instruction& instr);
    void vmulosh    (const Instruction& instr);
    void vrsqrtefp  (const Instruction& instr);
    void vmrglh     (const Instruction& instr);
    void vslw       (const Instruction& instr);
    void vmrglw     (const Instruction& instr);
    void vcmpgefp   (const Instruction& instr);
    void vpkswss    (const Instruction& instr);
    void vspltb     (const Instruction& instr);
    void vupkhsb    (const Instruction& instr);
    void vcmpgtuh   (const Instruction& instr);
    void vsplth     (const Instruction& instr);
    void vupkhsh    (const Instruction& instr);
    void vsrw       (const Instruction& instr);
    void vcmpgtuw   (const Instruction& instr);
    void vspltw     (const Instruction& instr);
    void vupklsb    (const Instruction& instr);
    void vcmpgtfp   (const Instruction& instr);
    void vupklsh    (const Instruction& instr);
    void vcfux      (const Instruction& instr);
    void vspltisb   (const Instruction& instr);
    void vaddshs    (const Instruction& instr);
    void vsrah      (const Instruction& instr);
    void vmulesh    (const Instruction& instr);
    void vcfsx      (const Instruction& instr);
    void vspltish   (const Instruction& instr);
    void vsraw      (const Instruction& instr);
    void vcmpgtsw   (const Instruction& instr);
    void vctuxs     (const Instruction& instr);
    void vspltisw   (const Instruction& instr);
    void vctsxs     (const Instruction& instr);
    void vand       (const Instruction& instr);
    void vmaxfp     (const Instruction& instr);
    void vsubuhm    (const Instruction& instr);
    void vandc      (const Instruction& instr);
    void vminfp     (const Instruction& instr);
    void vsubuwm    (const Instruction& instr);
    void vor        (const Instruction& instr);
    void vnor       (const Instruction& instr);
    void vxor       (const Instruction& instr);
    void vsubshs    (const Instruction& instr);
    // G_13
    void mcrf       (const Instruction& instr);
    void bclr       (const Instruction& instr);
    void crnor      (const Instruction& instr);
    void crandc     (const Instruction& instr);
    void crnand     (const Instruction& instr);
    void crand      (const Instruction& instr);
    void crorc      (const Instruction& instr);
    void cror       (const Instruction& instr);
    void bcctr      (const Instruction& instr);
    // G_1E
    void rldicl     (const Instruction& instr);
    void rldicr     (const Instruction& instr);
    void rldic      (const Instruction& instr);
    void rldimi     (const Instruction& instr);
    void rldcl      (const Instruction& instr);
    // G_1F
    void cmp        (const Instruction& instr);
    void lvsl       (const Instruction& instr);
    void subfc      (const Instruction& instr);
    void mulhdu     (const Instruction& instr);
    void addc       (const Instruction& instr);
    void mulhwu     (const Instruction& instr);
    void mfcr       (const Instruction& instr);
    void lwarx      (const Instruction& instr);
    void ldx        (const Instruction& instr);
    void lwzx       (const Instruction& instr);
    void cntlzw     (const Instruction& instr);
    void slw        (const Instruction& instr);
    void sld        (const Instruction& instr);
    void and_       (const Instruction& instr);
    void cmpl       (const Instruction& instr);
    void lvsr       (const Instruction& instr);
    void subf       (const Instruction& instr);
    void lwzux      (const Instruction& instr);
    void cntlzd     (const Instruction& instr);
    void andc       (const Instruction& instr);
    void lvewx      (const Instruction& instr);
    void mulhd      (const Instruction& instr);
    void mulhw      (const Instruction& instr);
    void ldarx      (const Instruction& instr);
    void lbzx       (const Instruction& instr);
    void lvx        (const Instruction& instr);
    void neg        (const Instruction& instr);
    void nor        (const Instruction& instr);
    void stvebx     (const Instruction& instr);
    void subfe      (const Instruction& instr);
    void adde       (const Instruction& instr);
    void mtcrf      (const Instruction& instr);
    void stdx       (const Instruction& instr);
    void stwcx      (const Instruction& instr);
    void stwx       (const Instruction& instr);
    void stvehx     (const Instruction& instr);
    void stdux      (const Instruction& instr);
    void stvewx     (const Instruction& instr);
    void addze      (const Instruction& instr);
    void stdcx      (const Instruction& instr);
    void stbx       (const Instruction& instr);
    void stvx       (const Instruction& instr);
    void mulld      (const Instruction& instr);
    void mullw      (const Instruction& instr);
    void stbux      (const Instruction& instr);
    void add        (const Instruction& instr);
    void lhzx       (const Instruction& instr);
    void xor_       (const Instruction& instr);
    void mfspr      (const Instruction& instr);
    void mftb       (const Instruction& instr);
    void sthx       (const Instruction& instr);
    void orc        (const Instruction& instr);
    void or_        (const Instruction& instr);
    void divdu      (const Instruction& instr);
    void divwu      (const Instruction& instr);
    void mtspr      (const Instruction& instr);
    void nand       (const Instruction& instr);
    void divd       (const Instruction& instr);
    void divw       (const Instruction& instr);
    void lvlx       (const Instruction& instr);
    void lwbrx      (const Instruction& instr);
    void lfsx       (const Instruction& instr);
    void srw        (const Instruction& instr);
    void srd        (const Instruction& instr);
    void lvrx       (const Instruction& instr);
    void lfdx       (const Instruction& instr);
    void stvlx      (const Instruction& instr);
    void stfsx      (const Instruction& instr);
    void stvrx      (const Instruction& instr);
    void stfdx      (const Instruction& instr);
    void lhbrx      (const Instruction& instr);
    void sraw       (const Instruction& instr);
    void srad       (const Instruction& instr);
    void srawi      (const Instruction& instr);
    void sradi      (const Instruction& instr);
    void extsh      (const Instruction& instr);
    void extsb      (const Instruction& instr);
    void extsw      (const Instruction& instr);
    void stfiwx     (const Instruction& instr);
    void dcbz       (const Instruction& instr);
    // G_3A
    void ld         (const Instruction& instr);
    void ldu        (const Instruction& instr);
    void lwa        (const Instruction& isntr);
    // G_3B
    void fdivs      (const Instruction& instr);
    void fsubs      (const Instruction& instr);
    void fadds      (const Instruction& instr);
    void fsqrts     (const Instruction& instr);
    void fmuls      (const Instruction& instr);
    void fmsubs     (const Instruction& instr);
    void fmadds     (const Instruction& instr);
    void fnmsubs    (const Instruction& instr);
    void fnmadds    (const Instruction& instr);
    // G_3E
    void std        (const Instruction& instr);
    void stdu       (const Instruction& instr);
    // G_3F
    void mffs       (const Instruction& instr);
    void mtfsf      (const Instruction& instr);
    void fcmpu      (const Instruction& instr);
    void frsp       (const Instruction& instr);
    void fctiwz     (const Instruction& instr);
    void fdiv       (const Instruction& instr);
    void fsub       (const Instruction& instr);
    void fadd       (const Instruction& instr);
    void fsqrt      (const Instruction& instr);
    void fsel       (const Instruction& instr);
    void fmul       (const Instruction& instr);
    void frsqrte    (const Instruction& instr);
    void fmr        (const Instruction& instr);
    void fmsub      (const Instruction& instr);
    void fmadd      (const Instruction& instr);
    void fnmsub     (const Instruction& instr);
    void fnmadd     (const Instruction& instr);
    void fneg       (const Instruction& instr);
    void fabs_      (const Instruction& instr);
    void fctid      (const Instruction& instr);
    void fctidz     (const Instruction& instr);
    void fcfid      (const Instruction& instr);
};
