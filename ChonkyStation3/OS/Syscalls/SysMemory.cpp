#include <Syscall.hpp>
#include "PlayStation3.hpp"


MAKE_LOG_FUNCTION(log, sysMemory);

u64 Syscall::sysMemoryAllocate() {
    const u32 size = ARG0;
    const u32 flags = ARG1;
    const u32 alloc_ptr = ARG2;
    log("sys_memory_allocate(size: 0x%08x, flags: 0x%08x, alloc_ptr: 0x%08x)\n", size, flags, alloc_ptr);

    auto block = ps3->mem.alloc(size, 0x30000000);
    ps3->mem.write<u32>(alloc_ptr, block->vaddr);

    return Result::CELL_OK;
}

u64 Syscall::sysMemoryGetUserMemorySize() {
    const u64 mem_info_ptr = ARG0;
    const auto available_mem = ps3->mem.ram.getAvailableMem();
    ps3->mem.write<u32>(mem_info_ptr, RAM_SIZE);
    ps3->mem.write<u32>(mem_info_ptr + 4, available_mem);

    log("sys_memory_get_user_memory_size(mem_info_ptr: 0x%08llx) [available memory: %lldMB]\n", mem_info_ptr, available_mem / 1024 / 1024);

    return Result::CELL_OK;
}