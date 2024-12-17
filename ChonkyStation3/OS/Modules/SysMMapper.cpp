#include "SysMMapper.hpp"
#include "PlayStation3.hpp"


u64 SysMMapper::sysMMapperAllocateMemory() {
    const u32 size = ARG0;
    const u64 flags = ARG1;
    const u32 addr_ptr = ARG2;
    
    auto block = ps3->mem.alloc(size);
    auto handle = ps3->handle_manager.request();
    block->handle = handle;
    ps3->mem.write<u32>(addr_ptr, handle);

    printf("sysMMapperAllocateMemory(size: 0x%08x, flags: 0x%016llx, addr_ptr: 0x%08x) [allocated memory addr: 0x%08llx]\n", size, flags, addr_ptr, block->vaddr);
    return Result::CELL_OK;
}