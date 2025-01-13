#pragma once

#include <common.hpp>
#include <opengl.hpp>
#include <unordered_map>
#include <xxhash.h>
#include <logger.hpp>


class ShaderCache {
public:
    MAKE_LOG_FUNCTION(log, shader_cache);

    // TODO: Might change this to an std::vector of "CachedShader" structs or something
    std::unordered_map<u64, OpenGL::Shader> cache;
    
    bool getShader(u64 hash, OpenGL::Shader& program) {
        if (cache.contains(hash)) {
            program = cache[hash];
            //log("Got cached shader: %016x\n", hash);
            return true;
        }
        return false;
    }

    void cacheShader(u64 hash, OpenGL::Shader& program) {
        cache[hash] = program;
        log("Cached new shader: %016x\n", hash);
    }

    u64 computeHash(u8* ptr, size_t size) {
        return XXH3_64bits(ptr, size);
    }
};