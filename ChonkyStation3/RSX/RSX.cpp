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

    glGenTextures(1, &tex.m_handle);
    glBindTexture(GL_TEXTURE_2D, tex.m_handle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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

void RSX::compileProgram() {
    ShaderCache::CachedShader cached_shader;
    // Check if our shaders were cached
    const u64 hash_vertex = shader_cache.computeHash((u8*)vertex_shader_data.data(), vertex_shader_data.size() * 4);
    if (!shader_cache.getShader(hash_vertex, cached_shader)) {
        // Shader wasn't cached, compile it and add it to the cache
        std::vector<u32> required_constants;
        auto vertex_shader = vertex_shader_decompiler.decompile(vertex_shader_data, required_constants);
        OpenGL::Shader new_shader;
        new_shader.create(vertex_shader, OpenGL::ShaderType::Vertex);
        shader_cache.cacheShader(hash_vertex, { new_shader, required_constants });
        vertex = new_shader;
    }
    else {
        vertex = cached_shader.shader;
        required_constants = cached_shader.required_constants.value();
    }

    const u64 hash_fragment = shader_cache.computeHash(fragment_shader_program.getData(ps3->mem), fragment_shader_program.getSize(ps3->mem));
    if (!shader_cache.getShader(hash_fragment, cached_shader)) {
        // Shader wasn't cached, compile it and add it to the cache
        auto fragment_shader = fragment_shader_decompiler.decompile(fragment_shader_program);
        OpenGL::Shader new_shader;
        new_shader.create(fragment_shader, OpenGL::ShaderType::Fragment);
        shader_cache.cacheShader(hash_fragment, { new_shader, std::nullopt });
        fragment = new_shader;
    }
    else {
        fragment = cached_shader.shader;
    }

    // Check if our shader program was cached
    const u64 hash_program = shader_cache.computeProgramHash(hash_vertex, hash_fragment);
    if (!shader_cache.getProgram(hash_program, program)) {
        // Program wasn't cached, link it and add it to the cache
        OpenGL::Program new_program;
        new_program.create({ vertex, fragment });
        shader_cache.cacheProgram(hash_program, new_program);
        program = new_program;
    }
}

void RSX::setupVAO() {
    for (auto& binding : vertex_array.bindings) {
        u32 offs_in_buf = binding.offset - vertex_array.getBase();
        // Setup VAO attribute
        vao.setAttributeFloat<float>(binding.index, binding.size, binding.stride, (void*)offs_in_buf, false);
        vao.enableAttribute(binding.index);
    }
}

void RSX::getVertices(u32 n_vertices, std::vector<u8>& vtx_buf, u32 start) {
    u32 vtx_buf_offs = vtx_buf.size();
    vtx_buf.resize(vtx_buf_offs + (vertex_array.size() * n_vertices));

    for (auto& binding : vertex_array.bindings) {
        u32 offs = binding.offset;
        u32 offs_in_buf = binding.offset - vertex_array.getBase();

        // Collect vertex data
        for (int i = start; i < start + n_vertices; i++) {
            for (int j = 0; j < binding.size; j++) {
                u32 x = ps3->mem.read<u32>(offs + j * 4);
                *(float*)&vtx_buf[vtx_buf_offs + offs_in_buf + binding.stride * i + j * 4] = reinterpret_cast<float&>(x);
            }
            //log("x: %f y: %f z: %f\n", *(float*)&vtx_buf[binding.stride * i + 0], *(float*)&vtx_buf[binding.stride * i + 4], *(float*)&vtx_buf[binding.stride * i + 8]);
            offs += binding.stride;
        }
    }
}

void RSX::uploadVertexConstants() {
    // Constants
    // TODO: don't upload constants if they weren't changed
    for (auto& i : required_constants) {
        u32 x = constants[i * 4 + 0];
        u32 y = constants[i * 4 + 1];
        u32 z = constants[i * 4 + 2];
        u32 w = constants[i * 4 + 3];
        glUniform4f(glGetUniformLocation(program.handle(), std::format("const_{}", i).c_str()), reinterpret_cast<float&>(x), reinterpret_cast<float&>(y), reinterpret_cast<float&>(z), reinterpret_cast<float&>(w));
    }
}

void RSX::uploadFragmentUniforms() {
    // Fragment uniforms
    for (auto& i : fragment_uniforms) {
        glUniform4f(glGetUniformLocation(program.handle(), i.name.c_str()), i.x, i.y, i.z, i.w);
    }
    fragment_uniforms.clear();
}

GLuint RSX::getTextureInternalFormat(u8 fmt) {
    switch (fmt & ~(CELL_GCM_TEXTURE_LN | CELL_GCM_TEXTURE_UN)) {

    case CELL_GCM_TEXTURE_B8:   return GL_RED;
    case CELL_GCM_TEXTURE_A8R8G8B8: return GL_RGBA;

    default:
        Helpers::panic("Unimplemented texture format 0x%02x (0x%02x)\n", fmt, fmt & ~(CELL_GCM_TEXTURE_LN | CELL_GCM_TEXTURE_UN));
    }
}

GLuint RSX::getTexturePixelFormat(u8 fmt) {
    switch (fmt & ~(CELL_GCM_TEXTURE_LN | CELL_GCM_TEXTURE_UN)) {

    case CELL_GCM_TEXTURE_B8:   return GL_RED;
    case CELL_GCM_TEXTURE_A8R8G8B8: return GL_RGBA;

    default:
        Helpers::panic("Unimplemented texture format 0x%02x (0x%02x)\n", fmt, fmt & ~(CELL_GCM_TEXTURE_LN | CELL_GCM_TEXTURE_UN));
    }
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

        case NV4097_DRAW_ARRAYS: {
            compileProgram();
            program.use();

            setupVAO();
            uploadVertexConstants();
            uploadFragmentUniforms();

            // Texture samplers
            glUniform1i(glGetUniformLocation(program.handle(), "tex"), 0);

            std::vector<u8> vtx_buf;
            int n_vertices = 0;
            for (auto& j : args) {
                const u32 first = j & 0xffffff;
                const u32 count = (j >> 24) + 1;
                n_vertices += count;

                log("Draw Arrays: first: %d count: %d\n", first, count);
                getVertices(count, vtx_buf, first);
            }
            
            glBufferData(GL_ARRAY_BUFFER, vtx_buf.size(), (void*)vtx_buf.data(), GL_STATIC_DRAW);
            glDrawArrays(GL_TRIANGLES, 0, n_vertices);

            vertex_array.bindings.clear();
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
            compileProgram();
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
            std::vector<u8> vtx_buf;
            getVertices(n_vertices, vtx_buf);
            
            setupVAO();
            uploadVertexConstants();
            uploadFragmentUniforms();

            // Texture samplers
            glUniform1i(glGetUniformLocation(program.handle(), "tex"), 0);

            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * 4, indices.data(), GL_STATIC_DRAW);
            glBufferData(GL_ARRAY_BUFFER, vtx_buf.size(), (void*)vtx_buf.data(), GL_STATIC_DRAW);
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

            vertex_array.bindings.clear();
            break;
        }

        case NV4097_SET_TEXTURE_OFFSET: {
            const u32 offs = args[0];
            const u8 loc = (args[1] & 0x3) - 1;
            const u32 addr = offsetAndLocationToAddress(offs, loc);
            const u8 dimension = (args[1] >> 4) & 0xf;
            const u8 format = (args[1] >> 8) & 0xff;
            // TODO: mipmap, cubemap
            log("Set texture: addr: 0x%08x, dimension: 0x%02x, format: 0x%02x, location: %s\n", addr, dimension, format, loc == 0 ? "RSX" : "MAIN");

            texture.addr = addr;
            texture.format = format;
            break;
        }

        case NV4097_SET_TEXTURE_IMAGE_RECT: {
            const u16 width = args[0] >> 16;
            const u16 height = args[0] & 0xfffff;
            log("Texture: width: %d, height: %d\n", width, height);
            
            texture.width = width;
            texture.height = height;

            const auto fmt = getTexturePixelFormat(texture.format);
            const auto internal = getTextureInternalFormat(texture.format);
            glActiveTexture(GL_TEXTURE0 + 0);
            glTexImage2D(GL_TEXTURE_2D, 0, fmt, width, height, 0, fmt, GL_UNSIGNED_BYTE, (void*)ps3->mem.getPtr(texture.addr));
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
            Helpers::debugAssert(args[1] == 0, "Set transform program load address: addr != 0 (0x%08x)\n", args[1]);
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
        Helpers::panic("GL error 0x%x\n", err);
    }
}