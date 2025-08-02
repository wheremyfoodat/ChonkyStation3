#pragma once

#include <PPU.hpp>
#include <PPU/Backends/PPUInterpreter.hpp>

#include <oaknut/code_block.hpp>
#include <oaknut/oaknut.hpp>

#include <optional>
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
    static constexpr XReg arg3 = X2;
    static constexpr XReg arg4 = X3;
    static constexpr XReg state_pointer = X29;

    static constexpr size_t executable_memory_size = 128_MB;
    // Allocate some extra space as padding for security purposes in the extremely unlikely occasion we manage to overflow the above size
    static constexpr size_t alloc_size = executable_memory_size + 0x10000;

    static constexpr u32 page_shift = 12;
    static constexpr u32 page_size = 1 << page_shift;
    static constexpr u32 page_mask = page_size - 1;

    // A function pointer to JIT-emitted code
    // JIT code returns the number of cycles executed, and takes a pointer to the JIT object in arg1
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
        // Displacement
        const int64_t disp = getPCOffset(xptr<const void*>(), ptr);

        // If the displacement can fit in a 26-bit int, that means we can emit a direct call to the address
        // Otherwise, load the address into a register and emit a blr
        const bool canDoDirectCall = isInt26(disp) && false;

        if (canDoDirectCall) {
            BL(disp);
        } else {
            MOV(scratch, (uintptr_t)ptr);
            BLR(scratch);
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
        std::memcpy(&function_ptr, &func, sizeof(T));
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

    // Register allocation info
    static constexpr int ALLOCATEABLE_GPR_COUNT = 15;

    // Our allocateable registers and the order they should be allocated
    // We prefer using non-volatile regs first
    const std::array<XReg, ALLOCATEABLE_GPR_COUNT> allocateableRegisters = {X21, X22, X23, X24, X25, X26, X27, X28,
                                                                                X9,  X10, X11, X12, X13, X14, X15};
    // Which of our allocateables are volatile?
    const std::array<XReg, 7> allocateableVolatiles = {X9,  X10, X11, X12, X13, X14, X15};
    // Which of them are not volatile?
    const std::array<XReg, 8> allocateableNonVolatiles = {X21, X22, X23, X24, X25, X26, X27, X28};

    enum class RegState { Unknown, Constant };
    enum class LoadingMode { DoNotLoad, Load };

    struct Reg {
        uint64_t val = 0;                    // The register's cached value used for constant propagation (Not implemented yet)
        RegState state = RegState::Unknown;  // Is this register's value a constant, or an unknown value?

        bool allocated = false;  // Has this guest register been allocated to a host reg?
        bool writeback = false;  // Does this register need to be written back to memory at the end of the block?
        
        // If a host reg has been allocated to this register, which reg is it?
        XReg allocatedReg = XZR;
        int allocatedRegIndex = 0;

        inline bool isConst() { return state == RegState::Constant; }
        inline bool isAllocated() { return allocated; }

        inline void markConst(uint32_t value) {
            Helpers::panic("JIT: Recompiler doesn't support constant propagation yet");
            val = value;
            state = RegState::Constant;
            writeback = false;  // Disable writeback in case the reg was previously allocated with writeback
            allocated = false;  // Unallocate register
        }

        // Note: It's important that markUnknown does not modify the val field as that would mess up codegen
        inline void markUnknown() { state = RegState::Unknown; }
        inline void setWriteback(bool wb) { writeback = wb; }
    };

    inline void markGPRConst(int index, uint32_t value) {
        gprs[index].markConst(value);
        if (hostGPRs[gprs[index].allocatedRegIndex].mappedReg == index) {
            hostGPRs[gprs[index].allocatedRegIndex].mappedReg =
                std::nullopt;  // Unmap the register on the host reg side too
        }
    }

    struct HostRegister {
        std::optional<int> mappedReg = std::nullopt;  // The register this is allocated to, if any
    };

    Reg gprs[32];
    std::array<HostRegister, ALLOCATEABLE_GPR_COUNT> hostGPRs;

    template <LoadingMode mode = LoadingMode::Load>
    void reserveReg(int index);
    void allocateReg(int reg);
    void allocateRegWithoutLoad(int reg);

    template <size_t T, size_t U>
    void allocateRegisters(std::array<uint, T> regsWithoutWb, std::array<uint, U> regsWithWb);

    void flushRegs();
    void spillRegisterCache();
    void prepareForCall();
    uint allocatedRegisterCount = 0;  // How many registers have been allocated in this block at this point?

    void pushNonVolatiles();
    void popNonVolatiles();

    void alloc_ra_wb_rt(Instruction instruction);
    void alloc_rs_wb_rt(Instruction instruction);

    void alloc_rs_rb_wb_ra(Instruction instruction);
    void alloc_ra_rb_wb_rt(Instruction instruction);

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
