#include "SysPrxForUser.hpp"
#include "PlayStation3.hpp"


u64 SysPrxForUser::sysProcessExit() {
    const s32 code = ARG0;
    printf("\n---------------------------\n");
    printf(  "PROCESS EXITED WITH CODE %d\n", code);
    exit(0);
}

u64 SysPrxForUser::sysProcessAtExitSpawn() {
    printf("sysProcessAtExitSpawn()\n");

    // TODO
    return Result::CELL_OK;
}

u64 SysPrxForUser::sysGetSystemTime() {
    printf("sysGetSystemTime()\n");

    // TODO
    return 0;
}

u64 SysPrxForUser::sysProcess_At_ExitSpawn() {
    printf("sysProcess_At_ExitSpawn()\n");

    // TODO
    return Result::CELL_OK;
}

u64 SysPrxForUser::sysSpinlockInitialize() {
    const u64 ptr = ARG0;
    printf("sysSpinlockInitialize(lock_ptr: 0x%08llx)\n", ptr);

    ps3->mem.write<u32>(ptr, 0);
    return Result::CELL_OK;
}

u64 SysPrxForUser::sysSpinlockLock() {
    const u64 ptr = ARG0;
    printf("sysSpinlockLock(lock_ptr: 0x%08llx)\n", ptr);

    if (ps3->mem.read<u32>(ptr))
        Helpers::panic("Tried to lock already locked lock\n");

    ps3->mem.write<u32>(ptr, 1);
    return Result::CELL_OK;
}

u64 SysPrxForUser::sysSpinlockUnlock() {
    const u64 ptr = ARG0;
    printf("sysSpinlockUnlock(lock_ptr: 0x%08llx)\n", ptr);

    // TODO: should I actually check for this?
    if (!ps3->mem.read<u32>(ptr))
        Helpers::panic("Tried to unlock already unlocked lock\n");

    ps3->mem.write<u32>(ptr, 0);
    return Result::CELL_OK;
}