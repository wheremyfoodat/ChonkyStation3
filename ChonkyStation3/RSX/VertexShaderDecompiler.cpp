#include "VertexShaderDecompiler.hpp"


std::string VertexShaderDecompiler::decompile(u32* shader_data, u32 start, std::vector<u32>& required_constants) {
    std::string shader_base =
R"(
#version 410 core


vec4 r[32];
ivec4 addr0;
ivec4 addr1;
vec4 cc0;
vec4 cc1;

// Viewport transformation
uniform vec3 viewport_offs;
uniform vec3 viewport_scale;
uniform ivec2 surface_clip;


)";
    std::string shader;
    std::string main = "";
    main.reserve(256_KB);
    for (int i = 0; i < 16; i++) used_inputs[i] = false;
    for (int i = 0; i < 16; i++) used_outputs[i] = false;
    inputs = "";
    outputs = "";
    constants = "";
    initialization = "";
    required_constants.clear();
    curr_constants = &required_constants;

    for (int i = start * 4; i < 512 * 4; i += 4) {
        VertexInstruction* instr = (VertexInstruction*)&shader_data[i];
        log("VEC: %s\n", vertex_vector_opcodes[instr->w1.vector_opc].c_str());
        if (instr->w1.scalar_opc) printf("SCA: %s\n", vertex_scalar_opcodes[instr->w1.scalar_opc].c_str());
    }

    // Start is an instruction index, each instruction is 4 words long, so we multiply by 4
    for (int i = start * 4; i < 512 * 4; i += 4) {
        VertexInstruction* instr = (VertexInstruction*)&shader_data[i];

        // Input data sources
        // Instructions can have up to 3 inputs (for example the multiply-add instruction)
        // They can be an input vector (i.e. "in_pos"), a data vector (each shader has 16 general purpose vectors to work with), or a constant register (uniforms)
        VertexSource src0 = { .raw = (instr->w1.src0_hi << 9) | instr->w2.src0_lo };
        VertexSource src1 = { .raw = instr->w2.src1 };
        VertexSource src2 = { .raw = (instr->w2.src2_hi << 11) | instr->w3.src2_lo };
        
        int num_lanes;
        const auto mask_str = mask(instr, num_lanes);
        const auto type = getType(num_lanes);
        std::string decompiled_dest = dest(instr, instr->w1.vector_opc == RSXVertex::VECTOR::ARL);
        std::string decompiled_src;

        switch (instr->w1.vector_opc) {
        case RSXVertex::VECTOR::NOP:    break;
        case RSXVertex::VECTOR::MOV: {
            decompiled_src = std::format("{}", source(src0, instr));
            break;
        }
        case RSXVertex::VECTOR::MUL: {
            decompiled_src = std::format("({} * {})", source(src0, instr), source(src1, instr));
            break;
        }
        case RSXVertex::VECTOR::ADD: {
            decompiled_src = std::format("({} + {})", source(src0, instr), source(src2, instr));
            break;
        }
        case RSXVertex::VECTOR::MAD: {
            decompiled_src = std::format("(({} * {}) + {})", source(src0, instr), source(src1, instr), source(src2, instr));
            break;
        }
        case RSXVertex::VECTOR::DP3: {
            decompiled_src = std::format("vec4(dot({}.xyz, {}.xyz))", source(src0, instr), source(src1, instr));
            break;
        }
        case RSXVertex::VECTOR::DPH: {
            decompiled_src = std::format("vec4(dot(vec4({}.xyz, 1.0), {}))", source(src0, instr), source(src1, instr));
            break;
        }
        case RSXVertex::VECTOR::DP4: {
            decompiled_src = std::format("vec4(dot({}, {}))", source(src0, instr), source(src1, instr));
            break;
        }
        case RSXVertex::VECTOR::MIN: {
            decompiled_src = std::format("min({}, {})", source(src0, instr), source(src1, instr));
            break;
        }
        case RSXVertex::VECTOR::MAX: {
            decompiled_src = std::format("max({}, {})", source(src0, instr), source(src1, instr));
            break;
        }
        case RSXVertex::VECTOR::SLT: {
            decompiled_src = std::format("lessThan({}, {})", source(src0, instr), source(src1, instr));
            break;
        }
        case RSXVertex::VECTOR::ARL: {
            decompiled_src = std::format("ivec4({})", source(src0, instr));
            break;
        }
        case RSXVertex::VECTOR::FRC: {
            decompiled_src = std::format("fract({})", source(src0, instr));
            break;
        }
        case RSXVertex::VECTOR::FLR: {
            decompiled_src = std::format("floor({})", source(src0, instr));
            break;
        }
        case RSXVertex::VECTOR::SGT: {
            decompiled_src = std::format("greaterThan({}, {})", source(src0, instr), source(src1, instr));
            break;
        }

        default:
            Helpers::panic("Unimplemented vertex vector instruction 0x%02x\n", (u8)instr->w1.vector_opc);
        }

        if (!instr->w0.saturate)
            main += std::format("{}{} = {}{};\n", decompiled_dest, mask_str, decompiled_src, mask_str);
        else
            main += std::format("{}{} = clamp({}{}, 0.0f, 1.0f);\n", decompiled_dest, mask_str, decompiled_src, mask_str);
        
        if (instr->w3.end) break;
    }
    main += R"(
// Apply viewport transformation
vec3 scale = viewport_scale;
vec3 offs  = viewport_offs;
vec2 half_clip = surface_clip / 2.0f;
scale.x /=  half_clip.x;
scale.y /= -half_clip.y;
offs.x  -= half_clip.x;
offs.x  /= half_clip.x;
offs.y  -= half_clip.y;
offs.y  /= half_clip.y;
gl_Position = vec4(fs_pos.xyz * scale + offs, fs_pos.w);
)";

    declareFunction("void main", initialization + "\n" + main, shader);

    shader_base += inputs + "\n";
    shader_base += outputs + "\n";
    shader_base += constants;
    std::string full_shader = shader_base + "\n\n" + shader;

    log("Decompiled vertex shader:\n");
    printf("%s\n", full_shader.c_str());

    return full_shader;
}

void VertexShaderDecompiler::declareFunction(std::string name, std::string code, std::string& shader) {
    shader += name + "() {\n";
    std::istringstream stream(code);
    for (std::string line; std::getline(stream, line); )
        shader += "\t" + line + "\n";
    shader += "}\n";
}

void VertexShaderDecompiler::markInputAsUsed(std::string name, int location) {
    if (used_inputs[location]) return;
    used_inputs[location] = true;
    inputs += "layout (location = " + std::to_string(location) + ") in vec4 " + name + ";\n";
}

void VertexShaderDecompiler::markOutputAsUsed(std::string name, int location) {
    if (used_outputs[location]) return;
    used_outputs[location] = true;
    if (location != 0)
        outputs += "layout (location = " + std::to_string(output_locations_map[location]) + ") out vec4 " + name + ";\n";
    else
        outputs += "vec4 " + name + ";\n";
    initialization += name + " = vec4(0.0f, 0.0f, 0.0f, 1.0f);\n";
}

void VertexShaderDecompiler::markConstantAsUsed(std::string name) {
    constants += "uniform vec4 " + name + ";\n";
}

std::string VertexShaderDecompiler::source(VertexSource& src, VertexInstruction* instr) {
    std::string source = "";

    switch (src.type) {
    case VERTEX_SOURCE_TYPE::TEMP: {
        source = "r[" + std::to_string(src.temp_src_idx) + "]";
        break;
    }
    case VERTEX_SOURCE_TYPE::INPUT: {
        const int idx = instr->w1.input_src_idx;
        source = input_names[idx];
        markInputAsUsed(source, idx);
        break;
    }
    case VERTEX_SOURCE_TYPE::CONST: {
        source = "const_" + std::to_string(instr->w1.const_src_idx);
        //source = "c[" + std::to_string(instr->w1.const_src_idx) + "]";
        if (std::find(curr_constants->begin(), curr_constants->end(), instr->w1.const_src_idx) == curr_constants->end()) {
            curr_constants->push_back(instr->w1.const_src_idx);
            markConstantAsUsed(source);
        }
        break;
    }
    default:
        printf("Unimplemented source type %d\n", (u32)src.type);
    }

    // Swizzle
    // Each field in the src contains a value ranging from 0 to 3, each corrisponding to a lane
    const std::string all = "xyzw";
    std::string swizzle = "    ";
    swizzle[0] = all[src.x];
    swizzle[1] = all[src.y];
    swizzle[2] = all[src.z];
    swizzle[3] = all[src.w];

    // We can omit ".xyzw"
    if (swizzle != all)
        source += "." + swizzle;
    
    if (src.neg)
        source = "-" + source;

    return source;
}

std::string VertexShaderDecompiler::dest(VertexInstruction* instr, bool is_addr_reg) {
    std::string dest = "";

    if (instr->w0.is_output) {
        Helpers::debugAssert(!is_addr_reg, "VertexShaderDecompiler: address register is dest\n");
        const int idx = instr->w3.dst;
        dest = output_names[idx];
        markOutputAsUsed(dest, idx);
    } else if (!is_addr_reg)
        if (instr->w0.temp_dst_idx == 0x3f)
            dest = "cc" + std::to_string(instr->w0.cond_reg_sel);
        else
            dest = "r[" + std::to_string(instr->w0.temp_dst_idx) + "]";
    else {
        Helpers::debugAssert(instr->w0.temp_dst_idx < 2, "VertexShaderDecompiler: address register idx is > 1\n");
        dest = "addr" + std::to_string(instr->w0.temp_dst_idx);
    }

    return dest;
}

std::string VertexShaderDecompiler::mask(VertexInstruction* instr, int& num_lanes) {
    std::string mask = ".";
    const std::string full = ".xyzw";
    num_lanes = 0;

    if (instr->w3.x) {
        mask += "x";
        num_lanes++;
    }
    if (instr->w3.y) {
        mask += "y";
        num_lanes++;
    }
    if (instr->w3.z) {
        mask += "z";
        num_lanes++;
    }
    if (instr->w3.w) {
        mask += "w";
        num_lanes++;
    }

    if (mask == full) mask = "";
    return mask;
}

std::string VertexShaderDecompiler::getType(const int num_lanes) {
    switch (num_lanes) {
    case 1: return "float";
    case 2: return "vec2";
    case 3: return "vec3";
    case 4: return "vec4";
    default:
        Helpers::panic("getType: %d lanes\n", num_lanes);
    }
}
