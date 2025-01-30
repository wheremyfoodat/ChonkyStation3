#include <Syscall.hpp>
#include "PlayStation3.hpp"


MAKE_LOG_FUNCTION(log, sysMMapper_sc);

u64 Syscall::sysMMapperAllocateAddress() {
    const u64 size = ARG0;
    const u64 flags = ARG1;
    const u64 alignment = ARG2;
    const u32 addr_ptr = ARG3;
    log("sysMMapperAllocateAddress(size: 0x%016llx, flags: 0x%016llx, alignment: 0x%016llx, addr_ptr: 0x%08x) UNIMPLEMENTED\n", size, flags, alignment, addr_ptr);

    ps3->mem.write<u32>(addr_ptr, ps3->mem.ram.virtual_base);

    return Result::CELL_OK;
}

u64 Syscall::sysMMapperSearchAndMapMemory() {
    const u32 start_addr = ARG0;
    const u32 handle = ARG1;
    const u64 flags = ARG2;
    const u32 addr_ptr = ARG3;
    log("sysMMapperSearchAndMapMemory(start_addr: 0x%08x, handle: 0x%08x, flags: 0x%016llx, addr_ptr: 0x%08x)\n", start_addr, handle, flags, addr_ptr);
    Helpers::debugAssert(start_addr == 0, "sysMMapperSearchAndMapMemory: start_addr != 0\n");
    auto block = ps3->mem.ram.findBlockWithHandle(handle);
    Helpers::debugAssert(block.first, "sysMMapperSearchAndMapMemory: unknown handle\n");

    // Find area to map block to
    u64 vaddr = ps3->mem.findNextAllocatableVaddr(block.second->size);
    auto entry = ps3->mem.mmap(vaddr, block.second->start, block.second->size);

    ps3->mem.write<u32>(addr_ptr, entry->vaddr);

    return Result::CELL_OK;
}