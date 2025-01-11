#include "ShaderDecompiler.hpp"


std::string ShaderDecompiler::decompileVertex(std::vector<u32> shader_data) {
    std::string shader_base =
R"(
#version 410 core


vec4 r[16];
uniform vec4 c[512]; // TODO: I'm unsure that this is the actual number


)";
    std::string shader;
    std::string main = "";
    for (int i = 0; i < 16; i++) used_inputs[i] = false;
    for (int i = 0; i < 16; i++) used_outputs[i] = false;
    inputs = "";
    outputs = "";


    for (int i = 0; i < shader_data.size(); i += 4) {
        VertexInstruction* instr = (VertexInstruction*)&shader_data[i];
        log("VEC: %s\n", vertex_vector_opcodes[instr->w1.vector_opc].c_str());
        if (instr->w1.scalar_opc) printf("SCA: %s\n", vertex_scalar_opcodes[instr->w1.scalar_opc].c_str());
    }

    for (int i = 0; i < shader_data.size(); i += 4) {
        VertexInstruction* instr = (VertexInstruction*)&shader_data[i];

        // Input data sources
        // Instructions can have up to 3 inputs (for example the multiply-add instruction)
        // They can be an input vector (i.e. "in_pos"), a data vector (each shader has 16 general purpose vectors to work with), or a constant register (uniforms)
        VertexSource src0 = { .raw = (instr->w1.src0_hi << 9) | instr->w2.src0_lo };
        VertexSource src1 = { .raw = instr->w2.src1 };
        VertexSource src2 = { .raw = (instr->w2.src2_hi << 11) | instr->w3.src2_lo };

        switch ((VERTEX_VECTOR_OPCODE)(u32)instr->w1.vector_opc) {
        case VERTEX_VECTOR_OPCODE::NOP:    break;
        case VERTEX_VECTOR_OPCODE::MOV: {
            int num_lanes;
            const auto mask_str = mask(instr, num_lanes);
            const auto type = getType(num_lanes);
            main += std::format("{}{} = {}({});\n", dest(instr), mask_str, type, source(src0, instr));
            break;
        }
        case VERTEX_VECTOR_OPCODE::DP4: {
            int num_lanes;
            const auto mask_str = mask(instr, num_lanes);
            const auto type = getType(num_lanes);
            main += std::format("{}{} = {}(dot({}, {}));\n", dest(instr), mask_str, type, source(src0, instr), source(src1, instr));
            break;
        }

        //default:
            //Helpers::panic("Unknown vertex vector instruction 0x%02x\n", (u8)instr->w1.vector_opc);
        }
    }
    main += "\ngl_Position = out_pos;\n";

    declareFunction("void main", main, shader);

    shader_base += inputs + "\n";
    shader_base += outputs;
    std::string full_shader = shader_base + "\n\n" + shader;

    log("Decompiled vertex shader:\n");
    log("%s\n", full_shader.c_str());

    return full_shader;
}

std::string ShaderDecompiler::decompileFragment(std::vector<u32> shader_data) {
    // TODO

    std::string full_shader =
R"(
#version 410 core


layout (location = 0) out vec4 out_col;


void main() {
    out_col = vec4(0.55f, 0.55f, 0.55f, 1.0f);
}
)";

    log("Decompiled fragment shader:\n");
    log("%s\n", full_shader.c_str());

    return full_shader;

}

void ShaderDecompiler::declareFunction(std::string name, std::string code, std::string& shader) {
    shader += name + "() {\n";
    std::istringstream stream(code);
    for (std::string line; std::getline(stream, line); )
        shader += "\t" + line + "\n";
    shader += "}\n";
}

void ShaderDecompiler::markInputAsUsed(std::string name, int location) {
    if (used_inputs[location]) return;
    used_inputs[location] = true;
    inputs += "layout (location = " + std::to_string(location) + ") in vec4 " + name + ";\n";
}

void ShaderDecompiler::markOutputAsUsed(std::string name, int location) {
    if (used_outputs[location]) return;
    used_outputs[location] = true;
    outputs += "layout (location = " + std::to_string(location) + ") out vec4 " + name + ";\n";
}

std::string ShaderDecompiler::source(VertexSource& src, VertexInstruction* instr) {
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
        //const std::string name = "const_" + std::to_string(instr->w1.const_src_idx);
        source = "c[" + std::to_string(instr->w1.const_src_idx) + "]";
        break;
    }
    default:
        printf("Unimplemented source type %d\n", (u32)src.type);
    }

    // Swizzle
    // Each field in the src contains a value ranging from 0 to 3, each corrisponding to a lane
    const std::string all = "xyzw";
    std::string swizzle = "";
    swizzle += all[src.x];
    swizzle += all[src.y];
    swizzle += all[src.z];
    swizzle += all[src.w];
    // We can omit ".xyzw"
    if (swizzle != all)
        source += "." + swizzle;

    return source;
}

std::string ShaderDecompiler::dest(VertexInstruction* instr) {
    std::string dest = "";

    if (instr->w0.is_output) {
        const int idx = instr->w3.dst;
        dest = output_names[idx];
        markOutputAsUsed(dest, idx);
    } else
        dest = "r[" + std::to_string(instr->w0.temp_dst_idx) + "]";

    return dest;
}

std::string ShaderDecompiler::mask(VertexInstruction* instr, int& num_lanes) {
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

std::string ShaderDecompiler::getType(const int num_lanes) {
    switch (num_lanes) {
    case 1: return "float";
    case 2: return "vec2";
    case 3: return "vec3";
    case 4: return "vec4";
    default:
        Helpers::panic("getType: %d lanes\n", num_lanes);
    }
}