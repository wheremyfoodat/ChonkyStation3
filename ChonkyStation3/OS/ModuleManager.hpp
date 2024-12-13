#pragma once

#include <common.hpp>
#include <unordered_map>
#include <Import.hpp>

#include <Modules/SysPrxForUser.hpp>
#include <Modules/SysThread.hpp>


// Circular dependency
class PlayStation3;

class ModuleManager {
public:
    ModuleManager(PlayStation3* ps3) : ps3(ps3), sysPrxForUser(ps3), sysThread(ps3) {}
    PlayStation3* ps3;

    void call(u32 nid);

    // Map address to import nid
    void registerImport(u32 addr, u32 nid);
    std::unordered_map<u32, u32> imports = {};
    std::unordered_map<u32, Import> import_map {
        { 0x2c847572, { "sysProcessAtExitSpawn",    std::bind(&SysPrxForUser::sysProcessAtExitSpawn, &sysPrxForUser) }},
        { 0x744680a2, { "sysThreadInitializeTLS",   std::bind(&SysPrxForUser::sysThreadInitializeTLS, &sysPrxForUser) }},
        { 0x8461e528, { "sysGetSystemTime",         std::bind(&SysPrxForUser::sysGetSystemTime, &sysPrxForUser) }},
        { 0x96328741, { "sysProcess_At_ExitSpawn",  std::bind(&SysPrxForUser::sysProcess_At_ExitSpawn, &sysPrxForUser) }},
        { 0x5267cb35, { "sysSpinlockUnlock",        std::bind(&SysPrxForUser::sysSpinlockUnlock, &sysPrxForUser) }},
        { 0x8c2bb498, { "sysSpinlockInitialize",    std::bind(&SysPrxForUser::sysSpinlockInitialize, &sysPrxForUser) }},
        { 0xa285139d, { "sysSpinlockLock",          std::bind(&SysPrxForUser::sysSpinlockLock, &sysPrxForUser) }},

        { 0x1573dc3f, { "sysLwMutexLock",           std::bind(&SysPrxForUser::sysLwMutexLock, &sysPrxForUser) }},
        { 0x1bc200f4, { "sysLwMutexUnlock",         std::bind(&SysPrxForUser::sysLwMutexUnlock, &sysPrxForUser) }},
        { 0x2f85c0ef, { "sysLwMutexCreate",         std::bind(&SysPrxForUser::sysLwMutexCreate, &sysPrxForUser) }},

        { 0x350d454e, { "sysThreadGetID",          std::bind(&SysThread::sysThreadGetID, &sysThread) }},
    };

    SysPrxForUser sysPrxForUser;
    SysThread sysThread;

    static Result stub(void* a) { Helpers::panic("Unimplemented function\n"); }
};