#pragma once

#include <common.hpp>
#include <string>
#include <sstream>
#include <BitField.hpp>
#include <unordered_map>
#include <format>


class ShaderDecompiler {
public:
    struct VertexInstruction {
        union {
            u32 raw;
            BitField<15, 6, u32> temp_dst_idx;
            BitField<30, 1, u32> is_output;
        } w0;
        union {
            u32 raw;
            BitField<0,  8,  u32> src0_hi;
            BitField<8,  4,  u32> input_src_idx;
            BitField<12, 10, u32> const_src_idx;
            BitField<22, 5,  u32> vector_opc;
            BitField<27, 5,  u32> scalar_opc;
        } w1;
        union {
            u32 raw;
            BitField<0, 6,  u32> src2_hi;
            BitField<6, 17, u32> src1;
            BitField<23, 9, u32> src0_lo;
        } w2;
        union {
            u32 raw;
            BitField<2,  5,  u32> dst;
            BitField<13, 1,  u32> w;
            BitField<14, 1,  u32> z;
            BitField<15, 1,  u32> y;
            BitField<16, 1,  u32> x;
            BitField<21, 11, u32> src2_lo;
        } w3;
    };

    union VertexSource {
        u32 raw;
        BitField<0, 2,  u32> type;
        BitField<2, 6,  u32> temp_src_idx; 
        BitField<8, 2,  u32> w;
        BitField<10, 2, u32> z;
        BitField<12, 2, u32> y;
        BitField<14, 2, u32> x;
        BitField<15, 1, u32> neg;
    };


    std::string decompileVertex(std::vector<u32> shader_data);
    std::string decompileFragment(std::vector<u32> shader_data);
    
    void declareFunction(std::string name, std::string code, std::string& shader);
    void markInputAsUsed(std::string name, int location);
    void markOutputAsUsed(std::string name, int location);
    //void markConstAsUsed(std::string name, int location);

    std::string source(VertexSource& src, VertexInstruction* instr);
    std::string dest(VertexInstruction* instr);
    std::string mask(VertexInstruction* instr, int& num_lanes);
    std::string getType(const int num_lanes);

    bool used_inputs[16];
    bool used_outputs[16];
    std::string inputs;
    std::string outputs;

    enum VERTEX_SOURCE_TYPE {
        TEMP = 1,
        INPUT = 2,
        CONST = 3
    };

    const std::string input_names[16] = {
        "in_pos",
        "in_weight",
        "in_normal",
        "in_diff_color",
        "in_spec_color",
        "in_fog",
        "in_point_size",
        "in_unk7",
        "in_tex0",
        "in_tex1",
        "in_tex2",
        "in_tex3",
        "in_tex4",
        "in_tex5",
        "in_tex6",
        "in_tex7"
    };

    const std::string output_names[16] = {
        "out_pos",
        "out_col0",
        "out_col1",
        "out_bfc0",
        "out_bfc1",
        "out_fog",
        "out_point_size",
        "out_tex0",
        "out_tex1",
        "out_tex2",
        "out_tex3",
        "out_tex4",
        "out_tex5",
        "out_tex6",
        "out_tex7",
        "out_unk15"
    };

    enum class VERTEX_SCALAR_OPCODE {
        NOP = 0x00, // No-Operation
        MOV = 0x01, // Move (copy)
        RCP = 0x02, // Reciprocal
        RCC = 0x03, // Reciprocal clamped
        RSQ = 0x04, // Reciprocal square root
        EXP = 0x05, // Exponential base 2 (low-precision)
        LOG = 0x06, // Logarithm base 2 (low-precision)
        LIT = 0x07, // Lighting calculation
        BRA = 0x08, // Branch
        BRI = 0x09, // Branch by CC register
        CAL = 0x0a, // Subroutine call
        CLI = 0x0b, // Subroutine call by CC register
        RET = 0x0c, // Return from subroutine
        LG2 = 0x0d, // Logarithm base 2
        EX2 = 0x0e, // Exponential base 2
        SIN = 0x0f, // Sine function
        COS = 0x10, // Cosine function
        BRB = 0x11, // Branch by Boolean constant
        CLB = 0x12, // Subroutine call by Boolean constant
        PSH = 0x13, // Push onto stack
        POP = 0x14, // Pop from stack
    };

    enum class VERTEX_VECTOR_OPCODE : u8 {
        NOP = 0x00, // No-Operation
        MOV = 0x01, // Move
        MUL = 0x02, // Multiply
        ADD = 0x03, // Addition
        MAD = 0x04, // Multiply-Add
        DP3 = 0x05, // 3-component Dot Product
        DPH = 0x06, // Homogeneous Dot Product
        DP4 = 0x07, // 4-component Dot Product
        DST = 0x08, // Calculate distance vector
        MIN = 0x09, // Minimum
        MAX = 0x0a, // Maximum
        SLT = 0x0b, // Set-If-LessThan
        SGE = 0x0c, // Set-If-GreaterEqual
        ARL = 0x0d, // Load to address register (round down)
        FRC = 0x0e, // Extract fractional part (fraction)
        FLR = 0x0f, // Round down (floor)
        SEQ = 0x10, // Set-If-Equal
        SFL = 0x11, // Set-If-False
        SGT = 0x12, // Set-If-GreaterThan
        SLE = 0x13, // Set-If-LessEqual
        SNE = 0x14, // Set-If-NotEqual
        STR = 0x15, // Set-If-True
        SSG = 0x16, // Convert positive values to 1 and negative values to -1
        TXL = 0x19, // Texture fetch
    };

    std::unordered_map<u8, std::string> vertex_vector_opcodes {
        { (u8)ShaderDecompiler::VERTEX_VECTOR_OPCODE::NOP, "NOP" },
        { (u8)ShaderDecompiler::VERTEX_VECTOR_OPCODE::MOV, "MOV" },
        { (u8)ShaderDecompiler::VERTEX_VECTOR_OPCODE::MUL, "MUL" },
        { (u8)ShaderDecompiler::VERTEX_VECTOR_OPCODE::ADD, "ADD" },
        { (u8)ShaderDecompiler::VERTEX_VECTOR_OPCODE::MAD, "MAD" },
        { (u8)ShaderDecompiler::VERTEX_VECTOR_OPCODE::DP3, "DP3" },
        { (u8)ShaderDecompiler::VERTEX_VECTOR_OPCODE::DPH, "DPH" },
        { (u8)ShaderDecompiler::VERTEX_VECTOR_OPCODE::DP4, "DP4" },
        { (u8)ShaderDecompiler::VERTEX_VECTOR_OPCODE::DST, "DST" },
        { (u8)ShaderDecompiler::VERTEX_VECTOR_OPCODE::MIN, "MIN" },
        { (u8)ShaderDecompiler::VERTEX_VECTOR_OPCODE::MAX, "MAX" },
        { (u8)ShaderDecompiler::VERTEX_VECTOR_OPCODE::SLT, "SLT" },
        { (u8)ShaderDecompiler::VERTEX_VECTOR_OPCODE::SGE, "SGE" },
        { (u8)ShaderDecompiler::VERTEX_VECTOR_OPCODE::ARL, "ARL" },
        { (u8)ShaderDecompiler::VERTEX_VECTOR_OPCODE::FRC, "FRC" },
        { (u8)ShaderDecompiler::VERTEX_VECTOR_OPCODE::FLR, "FLR" },
        { (u8)ShaderDecompiler::VERTEX_VECTOR_OPCODE::SEQ, "SEQ" },
        { (u8)ShaderDecompiler::VERTEX_VECTOR_OPCODE::SFL, "SFL" },
        { (u8)ShaderDecompiler::VERTEX_VECTOR_OPCODE::SGT, "SGT" },
        { (u8)ShaderDecompiler::VERTEX_VECTOR_OPCODE::SLE, "SLE" },
        { (u8)ShaderDecompiler::VERTEX_VECTOR_OPCODE::SNE, "SNE" },
        { (u8)ShaderDecompiler::VERTEX_VECTOR_OPCODE::STR, "STR" },
        { (u8)ShaderDecompiler::VERTEX_VECTOR_OPCODE::SSG, "SSG" },
        { (u8)ShaderDecompiler::VERTEX_VECTOR_OPCODE::TXL, "TXL" },
    };

    std::unordered_map<u8, std::string> vertex_scalar_opcodes {
        { (u8)ShaderDecompiler::VERTEX_SCALAR_OPCODE::NOP, "NOP" },
        { (u8)ShaderDecompiler::VERTEX_SCALAR_OPCODE::MOV, "MOV" },
        { (u8)ShaderDecompiler::VERTEX_SCALAR_OPCODE::RCP, "RCP" },
        { (u8)ShaderDecompiler::VERTEX_SCALAR_OPCODE::RCC, "RCC" },
        { (u8)ShaderDecompiler::VERTEX_SCALAR_OPCODE::RSQ, "RSQ" },
        { (u8)ShaderDecompiler::VERTEX_SCALAR_OPCODE::EXP, "EXP" },
        { (u8)ShaderDecompiler::VERTEX_SCALAR_OPCODE::LOG, "LOG" },
        { (u8)ShaderDecompiler::VERTEX_SCALAR_OPCODE::LIT, "LIT" },
        { (u8)ShaderDecompiler::VERTEX_SCALAR_OPCODE::BRA, "BRA" },
        { (u8)ShaderDecompiler::VERTEX_SCALAR_OPCODE::BRI, "BRI" },
        { (u8)ShaderDecompiler::VERTEX_SCALAR_OPCODE::CAL, "CAL" },
        { (u8)ShaderDecompiler::VERTEX_SCALAR_OPCODE::CLI, "CLI" },
        { (u8)ShaderDecompiler::VERTEX_SCALAR_OPCODE::RET, "RET" },
        { (u8)ShaderDecompiler::VERTEX_SCALAR_OPCODE::LG2, "LG2" },
        { (u8)ShaderDecompiler::VERTEX_SCALAR_OPCODE::EX2, "EX2" },
        { (u8)ShaderDecompiler::VERTEX_SCALAR_OPCODE::SIN, "SIN" },
        { (u8)ShaderDecompiler::VERTEX_SCALAR_OPCODE::COS, "COS" },
        { (u8)ShaderDecompiler::VERTEX_SCALAR_OPCODE::BRB, "BRB" },
        { (u8)ShaderDecompiler::VERTEX_SCALAR_OPCODE::CLB, "CLB" },
        { (u8)ShaderDecompiler::VERTEX_SCALAR_OPCODE::PSH, "PSH" },
        { (u8)ShaderDecompiler::VERTEX_SCALAR_OPCODE::POP, "POP" },
    };
};