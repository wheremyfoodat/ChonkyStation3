#pragma once

#include <common.hpp>
#include <unordered_map>
#include <Import.hpp>

#include <Modules/SysPrxForUser.hpp>


// Circular dependency
class PlayStation3;

class ModuleManager {
public:
    ModuleManager(PlayStation3* ps3) : ps3(ps3), sysPrxForUser(ps3) {}
    PlayStation3* ps3;

    void call(u32 nid);

    // Map address to import nid
    void registerImport(u32 addr, u32 nid);
    std::unordered_map<u32, u32> imports = {};
    std::unordered_map<u32, Import> import_map {
        { 0x2c847572, { "sysProcessAtExitSpawn",    std::bind(&SysPrxForUser::sysProcessAtExitSpawn, &sysPrxForUser) }},
        { 0x2f85c0ef, { "sysLwMutexCreate",         std::bind(&SysPrxForUser::sysLwMutexCreate, &sysPrxForUser) }},
        { 0x744680a2, { "sysThreadInitializeTLS",   std::bind(&SysPrxForUser::sysThreadInitializeTLS, &sysPrxForUser) }},
        { 0x8461e528, { "sysGetSystemTime",         std::bind(&SysPrxForUser::sysGetSystemTime, &sysPrxForUser) }},
        { 0x96328741, { "sysProcess_At_ExitSpawn",  std::bind(&SysPrxForUser::sysProcess_At_ExitSpawn, &sysPrxForUser) }},
    };

    SysPrxForUser sysPrxForUser;

    static Result stub(void* a) { Helpers::panic("Unimplemented function\n"); }
};