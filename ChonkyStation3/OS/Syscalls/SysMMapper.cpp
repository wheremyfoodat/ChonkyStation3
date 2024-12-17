#include <Syscall.hpp>
#include "PlayStation3.hpp"


u64 Syscall::sysMMapperAllocateAddress() {
    printf("sysMMapperAllocateAddress() UNIMPLEMENTED\n");
    // TODO: I don't know what this does
    return Result::CELL_OK;
}

u64 Syscall::sysMMapperSearchAndMapMemory() {
    // We map the memory when we allocate it, so we just do the "search" part here
    // Shouldn't be a problem
    // TODO: what is start_addr?
    const u32 start_addr = ARG0;
    const u32 handle = ARG1;
    const u64 flags = ARG2;
    const u32 addr_ptr = ARG3;
    printf("sysMMapperSearchAndMapMemory(start_addr: 0x%08x, handle: 0x%08x, flags: 0x%016llx, addr_ptr: 0x%08x)\n", start_addr, handle, flags, addr_ptr);
    Helpers::debugAssert(start_addr == 0, "sysMMapperSearchAndMapMemory: start_addr != 0\n");
    auto entry = ps3->mem.findMapEntryWithHandle(handle);
    Helpers::debugAssert(entry.first, "sysMMapperSearchAndMapMemory: unknown handle\n");

    ps3->mem.write<u32>(addr_ptr, entry.second->vaddr);

    return Result::CELL_OK;
}