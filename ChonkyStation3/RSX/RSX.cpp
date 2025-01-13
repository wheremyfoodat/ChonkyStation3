#include "RSX.hpp"
#include "PlayStation3.hpp"


RSX::RSX(PlayStation3* ps3) : ps3(ps3), gcm(ps3->module_manager.cellGcmSys), fragment_shader_decompiler(ps3) {
    OpenGL::setViewport(1280, 720);     // TODO: Get resolution from cellVideoOut
    OpenGL::setClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    OpenGL::clearColor();
    OpenGL::clearDepth();
    vao.create();
    vbo.create();
    vao.bind();
    vbo.bind();
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    OpenGL::enableDepth();
    OpenGL::setDepthFunc(OpenGL::DepthFunc::Less);
    OpenGL::disableScissor();
    OpenGL::setFillMode(OpenGL::FillMode::FillPoly);

    std::memset(constants, 0, 512 * 4);
}

u32 RSX::fetch32() {
    u32 data = ps3->mem.read<u32>(gcm.gcm_config.io_addr + curr_cmd);
    curr_cmd += 4;
    gcm.ctrl->get = gcm.ctrl->get + 4;  // Didn't overload += in BEField
    return data;
}

u32 RSX::offsetAndLocationToAddress(u32 offset, u8 location) {
    return offset + ((location == 0) ? ps3->module_manager.cellGcmSys.gcm_config.local_addr : ps3->module_manager.cellGcmSys.gcm_config.io_addr);
}

void RSX::runCommandList() {
    int cmd_count = gcm.ctrl->put - gcm.ctrl->get;
    if (cmd_count <= 0) return;

    log("Executing commands (%d bytes)\n", cmd_count);

    // Execute while get < put
    // We increment get as we fetch data from the FIFO
    while (gcm.ctrl->get < gcm.ctrl->put) {
        u32 cmd = fetch32();
        const auto cmd_num = cmd & 0x3ffff;
        const auto argc = (cmd >> 18) & 0x7ff;

        if (cmd & 0x20000000) { // jump
            curr_cmd = cmd & ~0x20000000;
            continue;
        }

        std::vector<u32> args;
        for (int i = 0; i < argc; i++)
            args.push_back(fetch32());

        if (command_names.contains(cmd_num))
            log("%s\n", command_names[cmd_num].c_str());

        switch (cmd_num) {

        case NV406E_SEMAPHORE_OFFSET:
        case NV4097_SET_SEMAPHORE_OFFSET: {
            semaphore_offset = args[0];
            break;
        }

        case NV4097_BACK_END_WRITE_SEMAPHORE_RELEASE: {
            const u32 val = (args[0] & 0xff00ff00) | ((args[0] & 0xff) << 16) | ((args[0] >> 16) & 0xff);
            ps3->mem.write<u32>(gcm.label_addr + semaphore_offset, val);
            break;
        }

        case NV406E_SEMAPHORE_ACQUIRE: {
            break;
        }

        case NV4097_SET_SHADER_PROGRAM: {
            fragment_shader_program.addr = offsetAndLocationToAddress(args[0] & ~3, (args[0] & 3) - 1);
            log("Fragment shader: address: 0x%08x\n", fragment_shader_program.addr);
            break;
        }

        case NV4097_SET_TRANSFORM_PROGRAM: {
            for (auto& i : args) vertex_shader_data.push_back(i);
            log("Vertex shader: uploading %d words (%d instructions)\n", args.size(), args.size() / 4);
            break;
        }

        case NV4097_SET_VERTEX_DATA_ARRAY_OFFSET + 4:
        case NV4097_SET_VERTEX_DATA_ARRAY_OFFSET + 8:
        case NV4097_SET_VERTEX_DATA_ARRAY_OFFSET + 12:
        case NV4097_SET_VERTEX_DATA_ARRAY_OFFSET + 16:
        case NV4097_SET_VERTEX_DATA_ARRAY_OFFSET + 20:
        case NV4097_SET_VERTEX_DATA_ARRAY_OFFSET + 24:
        case NV4097_SET_VERTEX_DATA_ARRAY_OFFSET + 28:
        case NV4097_SET_VERTEX_DATA_ARRAY_OFFSET + 32:
        case NV4097_SET_VERTEX_DATA_ARRAY_OFFSET + 36:
        case NV4097_SET_VERTEX_DATA_ARRAY_OFFSET + 40:
        case NV4097_SET_VERTEX_DATA_ARRAY_OFFSET + 44:
        case NV4097_SET_VERTEX_DATA_ARRAY_OFFSET + 48:
        case NV4097_SET_VERTEX_DATA_ARRAY_OFFSET + 52:
        case NV4097_SET_VERTEX_DATA_ARRAY_OFFSET + 56:
        case NV4097_SET_VERTEX_DATA_ARRAY_OFFSET + 60:
        case NV4097_SET_VERTEX_DATA_ARRAY_OFFSET: {
            const u32 offset = args[0] & 0x7fffffff;
            const u8 location = args[0] >> 31;
            curr_binding.offset = offsetAndLocationToAddress(offset, location);
            log("Vertex attribute %d: offset: 0x%08x\n", curr_binding.index, curr_binding.offset);
            vertex_array.bindings.push_back(curr_binding);
            break;
        }

        case NV4097_SET_VERTEX_DATA_ARRAY_FORMAT + 4:
        case NV4097_SET_VERTEX_DATA_ARRAY_FORMAT + 8:
        case NV4097_SET_VERTEX_DATA_ARRAY_FORMAT + 12:
        case NV4097_SET_VERTEX_DATA_ARRAY_FORMAT + 16:
        case NV4097_SET_VERTEX_DATA_ARRAY_FORMAT + 20:
        case NV4097_SET_VERTEX_DATA_ARRAY_FORMAT + 24:
        case NV4097_SET_VERTEX_DATA_ARRAY_FORMAT + 28:
        case NV4097_SET_VERTEX_DATA_ARRAY_FORMAT + 32:
        case NV4097_SET_VERTEX_DATA_ARRAY_FORMAT + 36:
        case NV4097_SET_VERTEX_DATA_ARRAY_FORMAT + 40:
        case NV4097_SET_VERTEX_DATA_ARRAY_FORMAT + 44:
        case NV4097_SET_VERTEX_DATA_ARRAY_FORMAT + 48:
        case NV4097_SET_VERTEX_DATA_ARRAY_FORMAT + 52:
        case NV4097_SET_VERTEX_DATA_ARRAY_FORMAT + 56:
        case NV4097_SET_VERTEX_DATA_ARRAY_FORMAT + 60:
        case NV4097_SET_VERTEX_DATA_ARRAY_FORMAT: {
            curr_binding.index = (cmd_num - NV4097_SET_VERTEX_DATA_ARRAY_FORMAT) >> 2;
            curr_binding.type = args[0] & 0xf;
            curr_binding.size = (args[0] >> 4) & 0xf;
            curr_binding.stride = (args[0] >> 8) & 0xff;
            log("Vertex attribute %d: size: %d, stride: 0x%02x, type: %d\n", curr_binding.index, curr_binding.size, curr_binding.stride, curr_binding.type);
            break;
        }

        case NV4097_SET_INDEX_ARRAY_ADDRESS: {
            const u32 location = args[1] & 0xf;   // Local or RSX
            const u32 addr = offsetAndLocationToAddress(args[0], location);
            const u8 type = (args[1] >> 4) & 0xf;
            index_array.addr = addr;
            index_array.type = type;
            log("Index array: addr: 0x%08x, type: %d, location: %s\n", addr, type, location == 0 ? "RSX" : "MAIN");
            break;
        }

        case NV4097_DRAW_INDEX_ARRAY: {
            // Check if our shaders were cached
            const u64 hash = shader_cache.computeHash((u8*)vertex_shader_data.data(), vertex_shader_data.size() * 4);
            if (!shader_cache.getShader(hash, vertex)) {
                // Shader wasn't cached, compile it and add to the cache
                auto vertex_shader = vertex_shader_decompiler.decompile(vertex_shader_data);
                OpenGL::Shader new_shader;
                new_shader.create(vertex_shader, OpenGL::ShaderType::Vertex);
                shader_cache.cacheShader(hash, new_shader);
                vertex = new_shader;
            }

            auto fragment_shader = fragment_shader_decompiler.decompile(fragment_shader_program);

            checkGLError();

            fragment.free();
            program.free();

            fragment.create(fragment_shader, OpenGL::ShaderType::Fragment);
            program.create({ vertex, fragment });
            program.use();

            std::vector<u32> indices;
            u32 highest_index = 0;

            for (auto& j : args) {
                const u32 first = j & 0xffffff;
                const u32 count = (j >> 24) + 1;
                log("Draw Index Array: first: %d count: %d\n", first, count);
                if (index_array.type == 1) {
                    for (int i = first; i < first + count; i++) {
                        const u16 index = ps3->mem.read<u16>(index_array.addr + i * 2);
                        indices.push_back(index);
                        if (index > highest_index) highest_index = index;
                    }
                }
                else {
                    Helpers::panic("Index buffer type 0\n");
                }
            }

            const auto n_vertices = highest_index + 1;
            log("\nVertex buffer: %d vertices\n", n_vertices);

            // Draw
            // TODO: This doesn't work with multiple attributes for now
            std::vector<u8> vtx_buf;
            vtx_buf.resize(vertex_array.size() * n_vertices);

            for (auto& binding : vertex_array.bindings) {
                u32 offs = binding.offset;
                u32 offs_in_buf = binding.offset - vertex_array.getBase();
                for (int i = 0; i < n_vertices; i++) {
                    u32 x = ps3->mem.read<u32>(offs + 0);
                    u32 y = ps3->mem.read<u32>(offs + 4);
                    u32 z = ps3->mem.read<u32>(offs + 8);
                    *(float*)&vtx_buf[offs_in_buf + binding.stride * i + 0] = reinterpret_cast<float&>(x);
                    *(float*)&vtx_buf[offs_in_buf + binding.stride * i + 4] = reinterpret_cast<float&>(y);
                    *(float*)&vtx_buf[offs_in_buf + binding.stride * i + 8] = reinterpret_cast<float&>(z);
                    //log("x: %f y: %f z: %f\n", *(float*)&vtx_buf[binding.stride * i + 0], *(float*)&vtx_buf[binding.stride * i + 4], *(float*)&vtx_buf[binding.stride * i + 8]);
                    offs += binding.stride;
                }
                vao.setAttributeFloat<float>(binding.index, binding.size, binding.stride, (void*)offs_in_buf, false);
                vao.enableAttribute(binding.index);
            }

            // Constants
            // TODO: don't upload constants if they weren't changed
            for (auto& i : vertex_shader_decompiler.required_constants) {
                u32 x = constants[i * 4 + 0];
                u32 y = constants[i * 4 + 1];
                u32 z = constants[i * 4 + 2];
                u32 w = constants[i * 4 + 3];
                glUniform4f(glGetUniformLocation(program.handle(), std::format("const_{}", i).c_str()), reinterpret_cast<float&>(x), reinterpret_cast<float&>(y), reinterpret_cast<float&>(z), reinterpret_cast<float&>(w));
            }

            // Fragment uniforms
            for (auto& i : fragment_uniforms) {
                glUniform4f(glGetUniformLocation(program.handle(), i.name.c_str()), i.x, i.y, i.z, i.w);
            }
            fragment_uniforms.clear();

            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * 4, indices.data(), GL_STATIC_DRAW);
            glBufferData(GL_ARRAY_BUFFER, vtx_buf.size(), (void*)vtx_buf.data(), GL_STATIC_DRAW);
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

            vertex_array.bindings.clear();
            break;
        }

        case NV4097_SET_SHADER_CONTROL: {
            fragment_shader_program.ctrl = args[0];
            log("Fragment shader: control: 0x%08x\n", fragment_shader_program.ctrl);
            break;
        }

        case NV4097_SET_COLOR_CLEAR_VALUE: {
            clear_color.r() = ((args[0] >>  0) & 0xff) / 255.0f;
            clear_color.g() = ((args[0] >>  8) & 0xff) / 255.0f;
            clear_color.b() = ((args[0] >> 16) & 0xff) / 255.0f;
            clear_color.a() = ((args[0] >> 24) & 0xff) / 255.0f;
            break;
        }

        case NV4097_CLEAR_SURFACE: {
            OpenGL::setClearColor(clear_color.r(), clear_color.g(), clear_color.b(), clear_color.a());
            OpenGL::clearColor();
            OpenGL::clearDepthAndStencil();
            break;
        }

        case NV4097_SET_TRANSFORM_PROGRAM_LOAD: {
            Helpers::debugAssert(args[0] == 0, "Set transform program load address: addr != 0\n");
            vertex_shader_data.clear();
            break;
        }

        case NV4097_SET_TRANSFORM_CONSTANT_LOAD: {
            const u32 start = args[0];
            for (int i = 1; i < args.size(); i++) constants[start * 4 + i - 1] = args[i];                

            log("Upload %d transform constants starting at %d\n", args.size() - 1, args[0]);
            for (int i = 1; i < args.size(); i++) {
                log("0x%08x (%f)\n", constants[start * 4 + i - 1], reinterpret_cast<float&>(constants[start * 4 + i - 1]));
            }
            break;
        }

        case NV4097_SET_VERTEX_ATTRIB_OUTPUT_MASK: {
            for (int i = 0; i < 22; i++) {
                if (args[0] & (1 << i)) {
                    fragment_shader_decompiler.enableInput(i);
                }
            }
            break;
        }

        case NV3062_SET_OFFSET_DESTIN: {
            dest_offset = args[0];
            log("Dest offset: 0x%08x\n", dest_offset);
            break;
        }

        case NV308A_POINT: {
            point_x = args[0] & 0xffff;
            point_y = args[0] >> 16;
            log("Point: { x: 0x%04x, y: 0x%04x }\n", point_x, point_y);
            break;
        }

        case NV308A_COLOR: {
            u32 addr = offsetAndLocationToAddress(dest_offset + (point_x << 2), 0);
            Helpers::debugAssert(args.size() <= 4, "NV308A_COLOR: args size > 4\n");
            float v[4] = { 0 };
            log("Color: addr: 0x%08x\n", addr);
            for (int i = 0; i < args.size(); i++) {
                u32 swapped = (args[i] >> 16) | (args[i] << 16);
                v[i] = reinterpret_cast<float&>(swapped);
                log("Uploaded float 0x%08x\n", args[i]);
            }
            const auto name = fragment_shader_decompiler.addUniform(addr);
            fragment_uniforms.push_back({ name, v[0], v[1], v[2], v[3] });
            break;
        }

        default:
            if (command_names.contains(cmd & 0x3ffff))
                log("Unimplemented RSX command %s\n", command_names[cmd_num].c_str());
            else
                log("Unimplemented RSX command 0x%08x (0x%08x)\n", cmd_num, cmd);
        }
    }
}

void RSX::checkGLError() {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        log("GL error 0x%x\n", err);
        exit(0);
    }
}