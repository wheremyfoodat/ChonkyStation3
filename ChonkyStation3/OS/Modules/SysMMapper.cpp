#include "SysMMapper.hpp"
#include "PlayStation3.hpp"


u64 SysMMapper::sysMMapperAllocateMemory() {
    const u32 size = ARG0;
    const u64 flags = ARG1;
    const u32 addr_ptr = ARG2;
    printf("sysMMapperAllocateMemory(size: 0x%08x, flags: 0x%016llx, addr_ptr: 0x%08x)\n", size, flags, addr_ptr);
    exit(0);
}