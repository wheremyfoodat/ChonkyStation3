#pragma once

#include <common.hpp>
#include <logger.hpp>
#include <BitField.hpp>
#include <string>
#include <FragmentShader.hpp>
#include <RSXShaderOpcodes.hpp>
#include <unordered_map>
#include <format>


// Circular dependency
class PlayStation3;

class FragmentShaderDecompiler {
public:
    FragmentShaderDecompiler(PlayStation3* ps3) : ps3(ps3) {}
    PlayStation3* ps3;
    
    struct FragmentInstruction {
        union {
            u32 raw;
            BitField<0,  1, u32> end;
            BitField<1,  6, u32> dest_idx;
            BitField<7,  1, u32> half;
            BitField<9,  1, u32> x;
            BitField<10, 1, u32> y;
            BitField<11, 1, u32> z;
            BitField<12, 1, u32> w;
            BitField<13, 4, u32> src_idx;
            BitField<24, 6, u32> opc;
        } dst;
        union {
            u32 raw;
            BitField<0,  2, u32> type;
            BitField<2,  6, u32> temp_src_idx;
            BitField<8,  1, u32> half;
            BitField<9,  2, u32> x;
            BitField<11, 2, u32> y;
            BitField<13, 2, u32> z;
            BitField<15, 2, u32> w;
            BitField<17, 1, u32> neg;
        } src0;
        union {
            u32 raw;
            BitField<0,  2, u32> type;
            BitField<2,  6, u32> temp_src_idx;
            BitField<8,  1, u32> half;
            BitField<9,  2, u32> x;
            BitField<11, 2, u32> y;
            BitField<13, 2, u32> z;
            BitField<15, 2, u32> w;
            BitField<17, 1, u32> neg;
            BitField<31, 1, u32> branch;
        } src1;
        union {
            u32 raw;
            BitField<0,  2,  u32> type;
            BitField<2,  6,  u32> temp_src_idx;
            BitField<8,  1,  u32> half;
            BitField<9,  2,  u32> x;
            BitField<11, 2,  u32> y;
            BitField<13, 2,  u32> z;
            BitField<15, 2,  u32> w;
            BitField<17, 1,  u32> neg;
            BitField<19, 11, u32> addr_reg;
            BitField<30, 1,  u32> use_index_reg;
        } src2;
    };

    MAKE_LOG_FUNCTION(log, fragment_shader);

    std::string decompile(FragmentShader& shader_program);
    FragmentInstruction fetchInstr(u32 addr);
    u32 fetch32(u32 addr);

    std::string addConstant(float x, float y, float z, float w);
    void declareFunction(std::string name, std::string code, std::string& shader);
    void markRegAsUsed(std::string name, int location);

    std::string source(FragmentInstruction& instr, int s);
    std::string dest(FragmentInstruction& instr);
    std::string mask(FragmentInstruction& instr, int& num_lanes);
    std::string getType(const int num_lanes);

    u32 curr_offs = 0;
    int next_constant = 0;
    std::string constants;
    bool used_inputs[16];
    bool used_regs[16];
    std::string inputs;
    std::string regs;

    enum FRAGMENT_SOURCE_TYPE {
        TEMP = 0,
        INPUT = 1,
        CONST = 2
    };

    std::unordered_map<u8, std::string> fragment_opcodes {
        { RSXFragment::NOP,          "NOP" },
        { RSXFragment::MOV,          "MOV" },
        { RSXFragment::MUL,          "MUL" },
        { RSXFragment::ADD,          "ADD" },
        { RSXFragment::MAD,          "MAD" },
        { RSXFragment::DP3,          "DP3" },
        { RSXFragment::DP4,          "DP4" },
        { RSXFragment::DST,          "DST" },
        { RSXFragment::MIN,          "MIN" },
        { RSXFragment::MAX,          "MAX" },
        { RSXFragment::SLT,          "SLT" },
        { RSXFragment::SGE,          "SGE" },
        { RSXFragment::SLE,          "SLE" },
        { RSXFragment::SGT,          "SGT" },
        { RSXFragment::SNE,          "SNE" },
        { RSXFragment::SEQ,          "SEQ" },
        { RSXFragment::FRC,          "FRC" },
        { RSXFragment::FLR,          "FLR" },
        { RSXFragment::KIL,          "KIL" },
        { RSXFragment::PK4,          "PK4" },
        { RSXFragment::UP4,          "UP4" },
        { RSXFragment::DDX,          "DDX" },
        { RSXFragment::DDY,          "DDY" },
        { RSXFragment::TEX,          "TEX" },
        { RSXFragment::TXP,          "TXP" },
        { RSXFragment::TXD,          "TXD" },
        { RSXFragment::RCP,          "RCP" },
        { RSXFragment::RSQ,          "RSQ" },
        { RSXFragment::EX2,          "EX2" },
        { RSXFragment::LG2,          "LG2" },
        { RSXFragment::LIT,          "LIT" },
        { RSXFragment::LRP,          "LRP" },
        { RSXFragment::STR,          "STR" },
        { RSXFragment::SFL,          "SFL" },
        { RSXFragment::COS,          "COS" },
        { RSXFragment::SIN,          "SIN" },
        { RSXFragment::PK2,          "PK2" },
        { RSXFragment::UP2,          "UP2" },
        { RSXFragment::POW,          "POW" },
        { RSXFragment::PKB,          "PKB" },
        { RSXFragment::UPB,          "UPB" },
        { RSXFragment::PK16,         "PK16" },
        { RSXFragment::UP16,         "UP16" },
        { RSXFragment::BEM,          "BEM" },
        { RSXFragment::PKG,          "PKG" },
        { RSXFragment::UPG,          "UPG" },
        { RSXFragment::DP2A,         "DP2A" },
        { RSXFragment::TXL,          "TXL" },
        { RSXFragment::TXB,          "TXB" },
        { RSXFragment::TEXBEM,       "TEXBEM" },
        { RSXFragment::TXPBEM,       "TXPBEM" },
        { RSXFragment::BEMLUM,       "BEMLUM" },
        { RSXFragment::REFL,         "REFL" },
        { RSXFragment::TIMESWTEX,    "TIMESWTEX" },
        { RSXFragment::DP2,          "DP2" },
        { RSXFragment::NRM,          "NRM" },
        { RSXFragment::DIV,          "DIV" },
        { RSXFragment::DIVSQ,        "DIVSQ" },
        { RSXFragment::LIF,          "LIF" },
        { RSXFragment::FENCT,        "FENCT" },
        { RSXFragment::FENCB,        "FENCB" },
        { RSXFragment::BRK,          "BRK" },
        { RSXFragment::CAL,          "CAL" },
        { RSXFragment::IFE,          "IFE" },
        { RSXFragment::LOOP,         "LOOP" },
        { RSXFragment::REP,          "REP" },
        { RSXFragment::RET,          "RET" },
    };
};