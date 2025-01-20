#include "SysPrxForUser.hpp"
#include "PlayStation3.hpp"


u64 SysPrxForUser::sysProcessExit() {
    const s32 code = ARG0;
    ps3->ppu->printState();
    printf("\n---------------------------\n");
    printf(  "PROCESS EXITED WITH CODE %d\n", code);
    exit(0);
}

u64 SysPrxForUser::sysProcessAtExitSpawn() {
    log("sysProcessAtExitSpawn()\n");

    // TODO
    return Result::CELL_OK;
}

u64 SysPrxForUser::sysStrlen() {
    const u32 str = ARG0;
    log("sysStrlen(str: 0x%08x)\n", str);

    return std::strlen((const char*)ps3->mem.getPtr(str));
}

u64 SysPrxForUser::sysGetSystemTime() {
    log("sysGetSystemTime()\n");

    // TODO
    return 0;
}

u64 SysPrxForUser::sysProcess_At_ExitSpawn() {
    log("sysProcess_At_ExitSpawn()\n");

    // TODO
    return Result::CELL_OK;
}

u64 SysPrxForUser::sysSpinlockInitialize() {
    const u64 ptr = ARG0;
    log("sysSpinlockInitialize(lock_ptr: 0x%08llx)\n", ptr);

    ps3->mem.write<u32>(ptr, 0);
    return Result::CELL_OK;
}

u64 SysPrxForUser::sysStrcpy() {
    const u32 dst = ARG0;
    const u32 src = ARG1;
    log("sysStrcpy(dst: 0x%08x, src: 0x%08x)\n", dst, src);

    std::strcpy((char*)ps3->mem.getPtr(dst), (char*)ps3->mem.getPtr(src));
    return dst;
}

u64 SysPrxForUser::sysSpinlockLock() {
    const u64 ptr = ARG0;
    log("sysSpinlockLock(lock_ptr: 0x%08llx)\n", ptr);

    if (ps3->mem.read<u32>(ptr))
        Helpers::panic("Tried to lock already locked lock\n");

    ps3->mem.write<u32>(ptr, 1);
    return Result::CELL_OK;
}

u64 SysPrxForUser::sysSpinlockUnlock() {
    const u64 ptr = ARG0;
    log("sysSpinlockUnlock(lock_ptr: 0x%08llx)\n", ptr);

    // TODO: should I actually check for this?
    if (!ps3->mem.read<u32>(ptr))
        Helpers::panic("Tried to unlock already unlocked lock\n");

    ps3->mem.write<u32>(ptr, 0);
    return Result::CELL_OK;
}

u64 SysPrxForUser::sysMemset() {
    const u32 dst = ARG0;
    const u32 val = ARG1;
    const u32 size = ARG2;
    log("sysMemset(dst: 0x%08x, val: 0x%08x, size: 0x%08x)\n", dst, val, size);

    std::memset(ps3->mem.getPtr(dst), val, size);
    return Result::CELL_OK;
}

u64 SysPrxForUser::sysMemcpy() {
    const u32 dst = ARG0;
    const u32 src = ARG1;
    const u32 size = ARG2;
    log("sysMemcpy(dst: 0x%08x, src: 0x%08x, size: 0x%08x)\n", dst, src, size);

    std::memcpy(ps3->mem.getPtr(dst), ps3->mem.getPtr(src), size);
    return Result::CELL_OK;
}

u64 SysPrxForUser::sysMemcmp() {
    const u32 buf1 = ARG0;
    const u32 buf2 = ARG1;
    const u32 size = ARG2;
    log("sysMemcmp(dst: 0x%08x, src: 0x%08x, size: 0x%08x)\n", buf1, buf2, size);

    return std::memcmp(ps3->mem.getPtr(buf1), ps3->mem.getPtr(buf2), size);
}