#include "SysPrxForUser.hpp"
#include "PlayStation3.hpp"


u64 SysPrxForUser::sysProcessAtExitSpawn() {
    printf("sysProcessAtExitSpawn()\n");

    // TODO
    return Result::CELL_OK;
}

// Allocate TLS memory, copy TLS image to the newly allocated area. Returns TLS address in R13
u64 SysPrxForUser::sysThreadInitializeTLS() {
    const u64 thread_id = ARG0;
    const u32 tls_seg_addr = ARG1;
    const u32 tls_seg_size = ARG2;
    const u32 tls_mem_size = ARG3;
    printf("sysThreadInitializeTLS(thread_id: %lld, tls_seg_addr: 0x%08x, tls_seg_size: 0x%08x, tls_mem_size: 0x%08x)\n", thread_id, tls_seg_addr, tls_seg_size, tls_mem_size);

    // Allocate TLS memory
    const u32 tls_addr = ps3->thread_manager.allocTLS(tls_mem_size);
    // Copy TLS image to allocated memory
    const u8* tls_image_ptr = ps3->mem.getPtr(tls_seg_addr);
    u8* tls_area_ptr = ps3->mem.getPtr(tls_addr);
    std::memcpy(tls_area_ptr, tls_image_ptr, tls_seg_size);

    ps3->ppu->state.gprs[13] = tls_addr;
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

u64 SysPrxForUser::sysLwMutexLock() {
    const u64 ptr = ARG0;
    const u64 timeout = ARG1;
    printf("sysLwMutexLock(ptr: 0x%08llx, timeout: %lld)\n", ptr, timeout);

    if (ps3->mem.read<u64>(ptr))
        // TODO: recursion
        Helpers::panic("Tried to lock already locked mutex\n");

    ps3->mem.write<u64>(ptr, 1);
    return Result::CELL_OK;
}

u64 SysPrxForUser::sysLwMutexUnlock() {
    const u64 ptr = ARG0;
    printf("sysLwMutexUnlock(ptr: 0x%08llx)\n", ptr);

    // TODO: should I actually check for this?
    if (!ps3->mem.read<u64>(ptr))
        Helpers::panic("Tried to unlock already unlocked mutex\n");

    ps3->mem.write<u64>(ptr, 0);
    return Result::CELL_OK;
}

u64 SysPrxForUser::sysLwMutexCreate() {
    const u64 ptr = ARG0;
    const u64 attrib_ptr = ARG1;
    SysLwMutexAttrib* attrib = (SysLwMutexAttrib*)ps3->mem.getPtr(attrib_ptr);
    auto name = Helpers::readString(attrib->name);
    printf("sysLwMutexCreate(ptr: 0x%08llx, attrib_ptr: 0x%08llx (protocol: 0x%08x, recursive: 0x%08x, name: \"%s\")\n", ptr, attrib_ptr, (u32)attrib->protocol, (u32)attrib->recursive, name.c_str());

    // TODO
    ps3->mem.write<u64>(ptr, 0);
    ps3->mem.write<u32>(ptr + 8, 0);
    ps3->mem.write<u32>(ptr + 12, 0);
    ps3->mem.write<u32>(ptr + 16, 0);
    return Result::CELL_OK;
}