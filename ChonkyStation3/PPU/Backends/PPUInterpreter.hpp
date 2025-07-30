#pragma once

#include <PPU.hpp>

#include <cmath>
#if _MSC_VER
#include <intrin.h>
#pragma intrinsic(_mul128)
#endif


// Circular dependency
class PlayStation3;

using namespace PPUTypes;

class PPUInterpreter : public PPU {
    void generateRotationMasks();

public:
    PPUInterpreter(Memory& mem, PlayStation3* ps3);
    PPUInterpreter(Memory& mem, PlayStation3* ps3, PPUTypes::State& state);

    int step() override;
    bool should_break = false;

    u64 rotation_mask[64][64];

    template<typename T>
    inline bool isShOK(int v) {
        return !(v >= sizeof(T) * 8);
    }

    template<typename T>
    inline T safeShl(T v, int sh) {
        return isShOK<T>(sh) ? (v << sh) : 0;
    }

    template<typename T>
    inline T safeShr(T v, int sh) {
        return isShOK<T>(sh) ? (v >> sh) : 0;
    }

    void printCallStack() override;
    std::unordered_map<u32, std::string> known_funcs;   // Used for debugging
    std::vector<std::pair<u32, u32>> call_stack;    // First: addr of function, second: addr the function is called from
    // Debug symbols
    void printFunctionCall();
    
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
