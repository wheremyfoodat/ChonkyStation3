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
    static constexpr XReg state_pointer = X29;

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

    u32 block_size = 0; // Size of block in instructions
    u32 recompiler_pc = 0; // Current PC we're compiling instructions from
    bool stop_compiling = false; // Flag for whether it's time to end the block

    void emitDispatcher();
    void emitBlockLookup();

    JITCallback compileBlock();
    void compileInstruction(Instruction instruction);

    void writebackPC(u32 newPC);

    template <typename T>
    T getLabelPointer(const oaknut::Label& label) {
        auto pointer = reinterpret_cast<u8*>(oaknut::CodeBlock::ptr()) + label.offset();
        return reinterpret_cast<T>(pointer);
    }

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
        // Displacement (in words)
        const int64_t disp = getPCOffset(xptr<const void*>(), ptr) / 4;

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
        // Displacement (in words)
        const int64_t disp = getPCOffset(xptr<const void*>(), ptr) / 4;

        // If the displacement can fit in a 26-bit int, that means we can emit a direct call to the address
        // Otherwise, load the address into a register and emit a br
        const bool canDoDirectJump = isInt26(disp);

        if (canDoDirectJump) {
            B(disp);
        } else {
            MOV(scratch, (uintptr_t)ptr);
            BR(scratch);
        }
    }

    // Should we continue compiling a block? Depends on how big the block currently is, and whether we've hit the block size limit
    bool shouldContinue() {
        static constexpr u32 maximum_block_size = 2048;
        return !stop_compiling; //&& block_size < maximum_block_size;
    }

    // Emit a call to a class member function, passing "thisObject" (+ an adjustment if necessary)
    // As the function's "this" pointer. Only works with classes with single, non-virtual inheritance
    // Hence the static asserts. Those are all we need though, thankfully.
    template <typename T>
    void emitMemberFunctionCall(T func, void* thisObject, XReg scratch) {
        void* function_ptr;
        uintptr_t this_ptr = reinterpret_cast<uintptr_t>(thisObject);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
        static_assert(sizeof(T) == 8, "[x64 JIT] Invalid size for member function pointer");
        std::memcpy(&function_ptr, &func, sizeof(T));
#else
        static_assert(sizeof(T) == 16, "[x64 JIT] Invalid size for member function pointer");
        uintptr_t arr[2];
        std::memcpy(arr, &func, sizeof(T));
        // First 8 bytes correspond to the actual pointer to the function
        function_ptr = reinterpret_cast<void*>(arr[0]);
        // Next 8 bytes correspond to the "this" pointer adjustment
        this_ptr += arr[1];
#endif

        MOV(arg1, this_ptr);
        call(function_ptr, scratch);
    }

    template <typename T>
    void emitInterpreterFunctionCall(T func, XReg scratch) {
        void* function_ptr;
        uintptr_t interpreter_ptr = reinterpret_cast<uintptr_t>(&interpreter);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
        static_assert(sizeof(T) == 8, "[x64 JIT] Invalid size for member function pointer");
        std::memcpy(&functionPtr, &func, sizeof(T));
#else
        static_assert(sizeof(T) == 16, "[x64 JIT] Invalid size for member function pointer");
        uintptr_t arr[2];
        std::memcpy(arr, &func, sizeof(T));
        // First 8 bytes correspond to the actual pointer to the function
        function_ptr = reinterpret_cast<void*>(arr[0]);
        // Next 8 bytes correspond to the "this" pointer adjustment
        interpreter_ptr += arr[1];
#endif

        uintptr_t interpreter_offset = ((uintptr_t)interpreter_ptr - (uintptr_t)this);
        ADD(arg1, state_pointer, interpreter_offset);
        call(function_ptr, scratch);
    }

public:
    PPUArm64(Memory& mem, PlayStation3* ps3);
    int step() override;
    bool should_break = false;
    
    // Main
    void mulli      (Instruction instr);
    void subfic     (Instruction instr);
    void cmpli      (Instruction instr);
    void cmpi       (Instruction instr);
    void addic      (Instruction instr);
    void addic_     (Instruction instr);
    void addi       (Instruction instr);
    void addis      (Instruction instr);
    void bc         (Instruction instr);
    void sc         (Instruction instr);
    void b          (Instruction instr);
    void rlwimi     (Instruction instr);
    void rlwinm     (Instruction instr);
    void rlwnm      (Instruction instr);
    void ori        (Instruction instr);
    void oris       (Instruction instr);
    void xori       (Instruction instr);
    void xoris      (Instruction instr);
    void andi       (Instruction instr);
    void andis      (Instruction instr);
    void lwz        (Instruction instr);
    void lwzu       (Instruction instr);
    void lbz        (Instruction instr);
    void lbzu       (Instruction instr);
    void stw        (Instruction instr);
    void stwu       (Instruction instr);
    void stb        (Instruction instr);
    void stbu       (Instruction instr);
    void lhz        (Instruction instr);
    void lhzu       (Instruction instr);
    void sth        (Instruction instr);
    void sthu       (Instruction instr);
    void lfs        (Instruction instr);
    void lfsu       (Instruction instr);
    void lfd        (Instruction instr);
    void lfdu       (Instruction instr);
    void stfs       (Instruction instr);
    void stfsu      (Instruction instr);
    void stfd       (Instruction instr);
    void stfdu      (Instruction instr);
    // G_04
    void vcmpequb   (Instruction instr);
    void vaddfp     (Instruction instr);
    void vmhraddshs (Instruction instr);
    void vmladduhm  (Instruction instr);
    void vmsumshm   (Instruction instr);
    void vsel       (Instruction instr);
    void vperm      (Instruction instr);
    void vsldoi     (Instruction instr);
    void vmaddfp    (Instruction instr);
    void vnmsubfp   (Instruction instr);
    void vadduhm    (Instruction instr);
    void vmulouh    (Instruction instr);
    void vsubfp     (Instruction instr);
    void vmrghh     (Instruction instr);
    void vadduwm    (Instruction instr);
    void vrlw       (Instruction instr);
    void vcmpequw   (Instruction instr);
    void vmrghw     (Instruction instr);
    void vcmpeqfp   (Instruction instr);
    void vrefp      (Instruction instr);
    void vpkshus    (Instruction instr);
    void vslh       (Instruction instr);
    void vmulosh    (Instruction instr);
    void vrsqrtefp  (Instruction instr);
    void vmrglh     (Instruction instr);
    void vslw       (Instruction instr);
    void vmrglw     (Instruction instr);
    void vcmpgefp   (Instruction instr);
    void vpkswss    (Instruction instr);
    void vspltb     (Instruction instr);
    void vupkhsb    (Instruction instr);
    void vcmpgtuh   (Instruction instr);
    void vsplth     (Instruction instr);
    void vupkhsh    (Instruction instr);
    void vsrw       (Instruction instr);
    void vcmpgtuw   (Instruction instr);
    void vspltw     (Instruction instr);
    void vupklsb    (Instruction instr);
    void vcmpgtfp   (Instruction instr);
    void vupklsh    (Instruction instr);
    void vcfux      (Instruction instr);
    void vspltisb   (Instruction instr);
    void vaddshs    (Instruction instr);
    void vsrah      (Instruction instr);
    void vmulesh    (Instruction instr);
    void vcfsx      (Instruction instr);
    void vspltish   (Instruction instr);
    void vsraw      (Instruction instr);
    void vcmpgtsw   (Instruction instr);
    void vctuxs     (Instruction instr);
    void vspltisw   (Instruction instr);
    void vctsxs     (Instruction instr);
    void vand       (Instruction instr);
    void vmaxfp     (Instruction instr);
    void vsubuhm    (Instruction instr);
    void vandc      (Instruction instr);
    void vminfp     (Instruction instr);
    void vsubuwm    (Instruction instr);
    void vor        (Instruction instr);
    void vnor       (Instruction instr);
    void vxor       (Instruction instr);
    void vsubshs    (Instruction instr);
    // G_13
    void mcrf       (Instruction instr);
    void bclr       (Instruction instr);
    void crnor      (Instruction instr);
    void crandc     (Instruction instr);
    void crnand     (Instruction instr);
    void crand      (Instruction instr);
    void crorc      (Instruction instr);
    void cror       (Instruction instr);
    void bcctr      (Instruction instr);
    // G_1E
    void rldicl     (Instruction instr);
    void rldicr     (Instruction instr);
    void rldic      (Instruction instr);
    void rldimi     (Instruction instr);
    void rldcl      (Instruction instr);
    // G_1F
    void cmp        (Instruction instr);
    void lvsl       (Instruction instr);
    void subfc      (Instruction instr);
    void mulhdu     (Instruction instr);
    void addc       (Instruction instr);
    void mulhwu     (Instruction instr);
    void mfcr       (Instruction instr);
    void lwarx      (Instruction instr);
    void ldx        (Instruction instr);
    void lwzx       (Instruction instr);
    void cntlzw     (Instruction instr);
    void slw        (Instruction instr);
    void sld        (Instruction instr);
    void and_       (Instruction instr);
    void cmpl       (Instruction instr);
    void lvsr       (Instruction instr);
    void subf       (Instruction instr);
    void lwzux      (Instruction instr);
    void cntlzd     (Instruction instr);
    void andc       (Instruction instr);
    void lvewx      (Instruction instr);
    void mulhd      (Instruction instr);
    void mulhw      (Instruction instr);
    void ldarx      (Instruction instr);
    void lbzx       (Instruction instr);
    void lvx        (Instruction instr);
    void neg        (Instruction instr);
    void nor        (Instruction instr);
    void stvebx     (Instruction instr);
    void subfe      (Instruction instr);
    void adde       (Instruction instr);
    void mtcrf      (Instruction instr);
    void stdx       (Instruction instr);
    void stwcx      (Instruction instr);
    void stwx       (Instruction instr);
    void stvehx     (Instruction instr);
    void stdux      (Instruction instr);
    void stvewx     (Instruction instr);
    void addze      (Instruction instr);
    void stdcx      (Instruction instr);
    void stbx       (Instruction instr);
    void stvx       (Instruction instr);
    void mulld      (Instruction instr);
    void mullw      (Instruction instr);
    void stbux      (Instruction instr);
    void add        (Instruction instr);
    void lhzx       (Instruction instr);
    void xor_       (Instruction instr);
    void mfspr      (Instruction instr);
    void mftb       (Instruction instr);
    void sthx       (Instruction instr);
    void orc        (Instruction instr);
    void or_        (Instruction instr);
    void divdu      (Instruction instr);
    void divwu      (Instruction instr);
    void mtspr      (Instruction instr);
    void nand       (Instruction instr);
    void divd       (Instruction instr);
    void divw       (Instruction instr);
    void lvlx       (Instruction instr);
    void lwbrx      (Instruction instr);
    void lfsx       (Instruction instr);
    void srw        (Instruction instr);
    void srd        (Instruction instr);
    void lvrx       (Instruction instr);
    void lfdx       (Instruction instr);
    void stvlx      (Instruction instr);
    void stfsx      (Instruction instr);
    void stvrx      (Instruction instr);
    void stfdx      (Instruction instr);
    void lhbrx      (Instruction instr);
    void sraw       (Instruction instr);
    void srad       (Instruction instr);
    void srawi      (Instruction instr);
    void sradi      (Instruction instr);
    void extsh      (Instruction instr);
    void extsb      (Instruction instr);
    void extsw      (Instruction instr);
    void stfiwx     (Instruction instr);
    void dcbz       (Instruction instr);
    // G_3A
    void ld         (Instruction instr);
    void ldu        (Instruction instr);
    void lwa        (Instruction isntr);
    // G_3B
    void fdivs      (Instruction instr);
    void fsubs      (Instruction instr);
    void fadds      (Instruction instr);
    void fsqrts     (Instruction instr);
    void fmuls      (Instruction instr);
    void fmsubs     (Instruction instr);
    void fmadds     (Instruction instr);
    void fnmsubs    (Instruction instr);
    void fnmadds    (Instruction instr);
    // G_3E
    void std        (Instruction instr);
    void stdu       (Instruction instr);
    // G_3F
    void mffs       (Instruction instr);
    void mtfsf      (Instruction instr);
    void fcmpu      (Instruction instr);
    void frsp       (Instruction instr);
    void fctiwz     (Instruction instr);
    void fdiv       (Instruction instr);
    void fsub       (Instruction instr);
    void fadd       (Instruction instr);
    void fsqrt      (Instruction instr);
    void fsel       (Instruction instr);
    void fmul       (Instruction instr);
    void frsqrte    (Instruction instr);
    void fmr        (Instruction instr);
    void fmsub      (Instruction instr);
    void fmadd      (Instruction instr);
    void fnmsub     (Instruction instr);
    void fnmadd     (Instruction instr);
    void fneg       (Instruction instr);
    void fabs_      (Instruction instr);
    void fctid      (Instruction instr);
    void fctidz     (Instruction instr);
    void fcfid      (Instruction instr);
};
