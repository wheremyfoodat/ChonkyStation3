#pragma once

#include <BitField.hpp>


namespace PPUTypes {

struct ConditionRegister {
    u32 raw;
    enum {
        Negative = 0b1000,
        Positive = 0b0100,
        Zero     = 0b0010
    };
    
    void setCRField(u8 n, u8 v, u8 so = 0) {
        const auto bit = 28 - (n * 4);  // CR Fields are reversed (CR0 is bits 28-31)
        raw &= ~(0b1111 << bit);
        raw |= (v | so) << bit;
    }

    template <typename T>
    void compareAndUpdateCRField(u8 n, T a, T b, u8 so = 0) {
        if (a < b) setCRField(n, Negative, so);
        else if (a > b) setCRField(n, Positive, so);
        else setCRField(n, Zero, so);
    }
};

struct State {
    u64 gprs[32] = { 0 };
    u64 pc = 0;
    u64 lr;
    ConditionRegister cr;
    u64 ctr;
};

union Instruction {
    u32 raw;
    BitField<0,  1,  u32> lk;
    BitField<0,  1,  u32> rc;           // lk == rc
    BitField<0,  2,  u32> g_3a_field;
    BitField<0,  2,  u32> g_3e_field;   // g_3a_field == g_3e_field
    BitField<0,  16, u32> ui;
    BitField<0,  16, u32> d;            // ui == d
    BitField<0,  16, u32> si;           // ui == si
    BitField<1,  1,  u32> aa;
    BitField<1,  1,  u32> sh_hi;        // aa == sh_hi
    BitField<1,  5,  u32> me_5;
    BitField<1,  10, u32> g_13_field;
    BitField<1,  10, u32> g_1f_field;   // g_13_field == g_1f_field
    BitField<2,  3,  u32> g_1e_field;
    BitField<2,  14, u32> ds;
    BitField<2,  14, u32> bd;           // ds == bd
    BitField<2,  24, u32> li;
    BitField<5,  6,  u32> mb_6;
    BitField<6,  5,  u32> mb_5;
    BitField<11, 3,  u32> bh;
    BitField<11, 5,  u32> rb;
    BitField<11, 5,  u32> sh_lo;        // rb == sh_lo
    BitField<11, 10, u32> spr;
    BitField<16, 5,  u32> ra;
    BitField<16, 5,  u32> bi;           // ra == bi
    BitField<21, 1,  u32> l;
    BitField<21, 5,  u32> rt;
    BitField<21, 5,  u32> rs;           // rt == rs
    BitField<21, 5,  u32> bo;           // rt == bo
    BitField<23, 3,  u32> bf;
    BitField<26, 6,  u32> opc;
};

enum Instructions {
    TDI     = 0x02,     // Trap Doubleword Immediate 
    TWI     = 0x03,     // Trap Word Immediate
    G_04    = 0x04,
    MULLI   = 0x07,     // Multiply Low Immediate
    SUBFIC  = 0x08,     // Subtract from Immediate Carrying
    DOZI    = 0x09,     // (deleted instruction)
    CMPLI   = 0x0a,     // Compare Logical Immediate
    CMPI    = 0x0b,     // Compare Immediate
    ADDIC   = 0x0c,     // Add Immediate Carrying
    ADDIC_  = 0x0d,     // Add Immediate Carrying and Record
    ADDI    = 0x0e,     // Add Immediate
    ADDIS   = 0x0f,     // Add Immediate Shifted
    BC      = 0x10,     // Branch Conditional
    SC      = 0x11,     // System Call
    B       = 0x12,     // Branch
    G_13    = 0x13,
    RLWIMI  = 0x14,     // Rotate Left Word Immediate then Mask Insert
    RLWINM  = 0x15,     // Rotate Left Word Immediate then AND with Mask
    RLWNM   = 0x17,     // Rotate Left Word then AND with Mask
    ORI     = 0x18,     // OR Immediate
    ORIS    = 0x19,     // OR Immediate Shifted
    XORI    = 0x1a,     // XOR Immediate
    XORIS   = 0x1b,     // XOR Immediate Shifted
    ANDI    = 0x1c,     // AND Immediate
    ANDIS   = 0x1d,     // AND Immediate Shifted
    G_1E    = 0x1e,
    G_1F    = 0x1f,
    LWZ     = 0x20,     // Load Word and Zero Indexed
    LWZU    = 0x21,     // Load Word and Zero with Update Indexed
    LBZ     = 0x22,     // Load Byte and Zero
    LBZU    = 0x23,     // Load Byte and Zero with Update
    STW     = 0x24,     // Store Word
    STWU    = 0x25,     // Store Word with Update
    STB     = 0x26,     // Store Byte
    STBU    = 0x27,     // Store Byte with Update
    LHZ     = 0x28,     // Load Halfword and Zero
    LHZU    = 0x29,     // Load Halfword and Zero with Update
    LHA     = 0x2a,     // Load Halfword Algebraic with Update
    LHAU    = 0x2b,     // Load Halfword Algebraic
    STH     = 0x2c,     // Store Halfword
    STHU    = 0x2d,     // Store Halfword with Update
    LMW     = 0x2e,     // Load Multiple Word
    STMW    = 0x2f,     // Store Multiple Word
    LFS     = 0x30,     // Load Floating-Point Single
    LFSU    = 0x31,     // Load Floating-Point Single with Update
    LFD     = 0x32,     // Load Floating-Point Double
    LFDU    = 0x33,     // Load Floating-Point Double with Update
    STFS    = 0x34,     // Store Floating-Point Single
    STFSU   = 0x35,     // Store Floating-Point Single with Update
    STFD    = 0x36,     // Store Floating-Point Double
    STFDU   = 0x37,     // Store Floating-Point Double with Update
    LFQ     = 0x38,
    LFQU    = 0x39,
    G_3A    = 0x3a,
    G_3B    = 0x3b,
    G_3E    = 0x3e,
    G_3F    = 0x3f,
};

enum G_04Opcodes {
    VXOR = 0x262,
};

enum G_13Opcodes {      // Field 21 - 30 
    MCRF    = 0x000,
    BCLR    = 0x010,
    CRNOR   = 0x021,
    CRANDC  = 0x081,
    ISYNC   = 0x096,
    CRXOR   = 0x0c1,
    CRNAND  = 0x0e1,
    CRAND   = 0x101,
    CREQV   = 0x121,
    CRORC   = 0x1a1,
    CROR    = 0x1c1,
    BCCTR   = 0x210,
};

enum G_1EOpcodes {      // Field 27 - 29
    RLDICL  = 0x00,     // Rotate Left Doubleword Immediate then Clear Left
    RLDICR  = 0x01,
    RLDIC   = 0x02,
    RLDIMI  = 0x03,
};

enum G_1FOpcodes {      // Field 21 - 30
    CMP     = 0x000,
    TW      = 0x004,
    LVEBX   = 0x007,    // Load Vector Element Byte Indexed
    SUBFC   = 0x008,    // Subtract from Carrying
    MULHDU  = 0x009,
    ADDC    = 0x00a,
    MULHWU  = 0x00b,
    MFOCRF  = 0x013,
    LWARX   = 0x014,
    LDX     = 0x015,
    LWZX    = 0x017,
    SLW     = 0x018,
    CNTLZW  = 0x01a,
    SLD     = 0x01b,
    AND     = 0x01c,
    CMPL    = 0x020,    // Compare Logical
    LVEHX   = 0x027,    // Load Vector Element Halfword Indexed
    SUBF    = 0x028,
    LDUX    = 0x035,    // Load Doubleword with Update Indexed
    DCBST   = 0x036,
    CNTLZD  = 0x03a,
    ANDC    = 0x03c,
    LVEWX   = 0x047,    // Load Vector Element Word Indexed
    MULHD   = 0x049,
    MULHW   = 0x04b,
    LDARX   = 0x054,
    DCBF    = 0x056,
    LBZX    = 0x057,
    LVX     = 0x067,
    NEG     = 0x068,
    LBZUX   = 0x077,
    NOR     = 0x07c,
    SUBFE   = 0x088,    // Subtract from Extended
    ADDE    = 0x08a,
    MTOCRF  = 0x090,
    STDX    = 0x095,
    STWCX_  = 0x096,
    STWX    = 0x097,
    STDUX   = 0x0b5,
    ADDZE   = 0x0ca,
    STDCX_  = 0x0d6,
    STBX    = 0x0d7,
    STVX    = 0x0e7,
    MULLD   = 0x0e9,
    ADDME   = 0x0ea,
    MULLW   = 0x0eb,
    DCBTST  = 0x0f6,
    DOZ     = 0x108,
    ADD     = 0x10a,
    DCBT    = 0x116,
    LHZX    = 0x117,
    EQV     = 0x11c,
    ECIWX   = 0x136,
    LHZUX   = 0x137,
    XOR     = 0x13c,
    MFSPR   = 0x153,
    LHAX    = 0x157,
    ABS     = 0x168,
    MFTB    = 0x173,
    LHAUX   = 0x177,
    STHX    = 0x197,    // Store Halfword Indexed
    ORC     = 0x19c,    // OR with Complement
    ECOWX   = 0x1b6,
    OR      = 0x1bc,
    DIVDU   = 0x1c9,
    DIVWU   = 0x1cb,
    MTSPR   = 0x1d3,
    DCBI    = 0x1d6,
    DIVD    = 0x1e9,
    DIVW    = 0x1eb,
    LWBRX   = 0x216,
    LFSX    = 0x217,
    SRW     = 0x218,
    SRD     = 0x21b,
    LFSUX   = 0x237,
    SYNC    = 0x256,
    LFDX    = 0x257,
    LFDUX   = 0x277,
    STFSX   = 0x297,
    LHBRX   = 0x316,
    SRAW    = 0x318,
    SRAD    = 0x31A,
    SRAWI   = 0x338,
    SRADI1  = 0x33a, 
    SRADI2  = 0x33b, 
    EIEIO   = 0x356,
    EXTSH   = 0x39a,
    EXTSB   = 0x3ba,
    STFIWX  = 0x3d7,
    EXTSW   = 0x3da,
    ICBI    = 0x3d6,
    DCBZ    = 0x3f6,
};

enum G_3AOpcodes {      // Field 30 - 31
    LD  = 0x00,
    LDU = 0x01,
};

enum G_3BOpcodes {      // Field 26 - 30
    FDIVS   = 0x12,
    FSUBS   = 0x14,
    FADDS   = 0x15,
    FSQRTS  = 0x16,
    FRES    = 0x18,
    FMULS   = 0x19,
    FMSUBS  = 0x1c,
    FMADDS  = 0x1d,
    FNMSUBS = 0x1e,
    FNMADDS = 0x1f,
};

enum G_3EOpcodes {      // Field 30 - 31
    STD     = 0x00,     // Store Doubleword
    STDU    = 0x01,     // Store Doubleword with Update
};

enum G_3FOpcodes {      // Field 21 - 30
    MTFSB1  = 0x026,
    MCRFS   = 0x040,
    MTFSB0  = 0x046,
    MTFSFI  = 0x086,
    MFFS    = 0x247,
    MTFSF   = 0x2c7,
    FCMPU   = 0x000,
    FRSP    = 0x00c,
    FCTIW   = 0x00e,
    FCTIWZ  = 0x00f,
    FDIV    = 0x012,
    FSUB    = 0x014,
    FADD    = 0x015,
    FSQRT   = 0x016,
    FSEL    = 0x017,
    FMUL    = 0x019,
    FRSQRTE = 0x01a,
    FMSUB   = 0x01c,
    FMADD   = 0x01d,
    FNMSUB  = 0x01e,
    FNMADD  = 0x01f,
    FCMPO   = 0x020,
    FNEG    = 0x028,
    FMR     = 0x048,
    FNABS   = 0x088,
    FABS    = 0x108,
    FCTID   = 0x32e,
    FCTIDZ  = 0x32f,
    FCFID   = 0x34e,
};

} // End namespace PPUTypes