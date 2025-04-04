#include <Syscall.hpp>
#include "PlayStation3.hpp"


MAKE_LOG_FUNCTION(log_sys_memory, sys_memory);

u64 Syscall::sys_memory_allocate() {
    const u32 size = ARG0;
    const u32 flags = ARG1;
    const u32 alloc_ptr = ARG2;
    log_sys_memory("sys_memory_allocate(size: 0x%08x, flags: 0x%08x, alloc_ptr: 0x%08x)\n", size, flags, alloc_ptr);

    auto block = ps3->mem.alloc(size, 0x20000000);
    ps3->mem.write<u32>(alloc_ptr, block->vaddr);

    return Result::CELL_OK;
}

u64 Syscall::sys_memory_free() {
    const u32 start_addr = ARG0;
    log_sys_memory("sys_memory_free(start_addr: 0x%08x)\n", start_addr);

    auto entry = ps3->mem.isMapped(start_addr);
    Helpers::debugAssert(entry.first, "sys_memory_free: tried to free unmapped memory\n");

    ps3->mem.free(entry.second);

    return Result::CELL_OK;
}

u64 Syscall::sys_memory_get_user_memory_size() {
    const u64 mem_info_ptr = ARG0;
    const auto available_mem = ps3->mem.ram.getAvailableMem();
    ps3->mem.write<u32>(mem_info_ptr, RAM_SIZE);
    ps3->mem.write<u32>(mem_info_ptr + 4, available_mem);

    log_sys_memory("sys_memory_get_user_memory_size(mem_info_ptr: 0x%08llx) [available memory: %lldMB]\n", mem_info_ptr, available_mem / 1024 / 1024);

    return Result::CELL_OK;
}