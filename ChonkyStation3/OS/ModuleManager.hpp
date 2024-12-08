#pragma once

#include <common.hpp>
#include <unordered_map>
#include <Module.hpp>


class ModuleManager {
public:
    ModuleManager() {}

    void call(u32 nid);

    // Map address to import nid
    void registerImport(u32 addr, u32 nid);
    std::unordered_map<u32, u32> imports = {};
    static inline std::unordered_map<u32, std::string> import_names {
        { 0x744680a2, "sys_initialize_tls" }
    };
};