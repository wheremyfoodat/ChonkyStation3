#include <Syscall.hpp>
#include "PlayStation3.hpp"


u64 Syscall::sysMMapperAllocateAddress() {
    const u64 size = ARG0;
    const u64 flags = ARG1;
    const u64 alignment = ARG2;
    const u32 addr_ptr = ARG3;
    printf("sysMMapperAllocateAddress(size: 0x%016llx, flags: 0x%016llx, alignment: 0x%016llx, addr_ptr: 0x%08x) UNIMPLEMENTED\n", size, flags, alignment, addr_ptr);
    // I think what this does is, it looks for a memory region big enough to fit size, and returns the address in addr_ptr?
    // I should be able to just emulate this as a simple alloc?
    // TODO: I think I'm wrong
    //auto block = ps3->mem.alloc(size);
    //ps3->mem.write<u32>(addr_ptr, block->vaddr);

    return Result::CELL_OK;
}

u64 Syscall::sysMMapperSearchAndMapMemory() {
    // We map the memory when we allocate it, so we just do the "search" part here
    // Shouldn't be a problem
    // TODO: what is start_addr? Well, I guess it's the address I'm supposed to begin searching from. Just assert for now
    const u32 start_addr = ARG0;
    const u32 handle = ARG1;
    const u64 flags = ARG2;
    const u32 addr_ptr = ARG3;
    printf("sysMMapperSearchAndMapMemory(start_addr: 0x%08x, handle: 0x%08x, flags: 0x%016llx, addr_ptr: 0x%08x)\n", start_addr, handle, flags, addr_ptr);
    Helpers::debugAssert(start_addr == 0, "sysMMapperSearchAndMapMemory: start_addr != 0\n");
    auto entry = ps3->mem.ram.findMapEntryWithHandle(handle);
    Helpers::debugAssert(entry.first, "sysMMapperSearchAndMapMemory: unknown handle\n");

    ps3->mem.write<u32>(addr_ptr, entry.second->vaddr);

    return Result::CELL_OK;
}