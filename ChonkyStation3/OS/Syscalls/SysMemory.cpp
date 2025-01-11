#include <Syscall.hpp>
#include "PlayStation3.hpp"


MAKE_LOG_FUNCTION(log, sysMemory);

u64 Syscall::sysMemoryGetUserMemorySize() {
    const u64 mem_info_ptr = ARG0;
    const auto available_mem = ps3->mem.ram.getAvailableMem();
    ps3->mem.write<u32>(mem_info_ptr, RAM_SIZE);
    ps3->mem.write<u32>(mem_info_ptr + 4, available_mem);

    log("sysMemoryGetUserMemorySize(mem_info_ptr: 0x%08llx) [available memory: %lldMB]\n", mem_info_ptr, available_mem / 1024 / 1024);

    return Result::CELL_OK;
}