#pragma once

#include <common.hpp>
#include <opengl.hpp>
#include <unordered_map>
#include <xxhash.h>
#include <logger.hpp>


class ShaderCache {
public:
    MAKE_LOG_FUNCTION(log, shader_cache);

    struct ProgramHash {
        u64 hash_vertex;
        u64 hash_fragment;

        bool operator==(const ProgramHash& other) {
            return hash_vertex == other.hash_vertex && hash_fragment == other.hash_fragment;
        }
    };
    
    bool getShader(u64 hash, OpenGL::Shader& shader) {
        if (shader_cache.contains(hash)) {
            shader = shader_cache[hash];
            //log("Got cached shader: %016x\n", hash);
            return true;
        }
        return false;
    }

    bool getProgram(u64 hash, OpenGL::Program& program) {
        if (program_cache.contains(hash)) {
            program = program_cache[hash];
            //log("Got cached program: %016x\n", hash);
            return true;
        }
        return false;
    }

    void cacheShader(u64 hash, OpenGL::Shader& shader) {
        shader_cache[hash] = shader;
        log("Cached new shader: %016x\n", hash);
    }

    void cacheProgram(u64 hash, OpenGL::Program& program) {
        program_cache[hash] = program;
        log("Cached new program: %016x\n", hash);
    }

    u64 computeHash(u8* ptr, size_t size) {
        return XXH3_64bits(ptr, size);
    }

    u64 computeProgramHash(u64 hash_vertex, u64 hash_fragment) {
        u64 hashes[2] = { hash_vertex, hash_fragment };
        return computeHash((u8*)&hashes[0], sizeof(u64) * 2);
    }

private:
    // TODO: Might change this to an std::vector of "CachedShader" structs or something
    std::unordered_map<u64, OpenGL::Shader> shader_cache;
    std::unordered_map<u64, OpenGL::Program> program_cache;
};