#pragma once

#include <common.hpp>
#include <CellTypes.hpp>
#include <BEField.hpp>


// Circular dependency
class PlayStation3;

using namespace CellTypes;

class SysPrxForUser {
public:
    SysPrxForUser(PlayStation3* ps3) : ps3(ps3) {}
    PlayStation3* ps3;

    struct SysLwMutexAttrib {
        BEField<u32> protocol;
        BEField<u32> recursive;
        u8 name[8];
    };

    u64 sysProcessAtExitSpawn();
    u64 sysThreadInitializeTLS();
    u64 sysGetSystemTime();
    u64 sysProcess_At_ExitSpawn();
    u64 sysSpinlockInitialize();
    u64 sysSpinlockLock();
    u64 sysSpinlockUnlock();

    u64 sysLwMutexCreate();
    u64 sysLwMutexLock();
    u64 sysLwMutexUnlock();
};