#pragma once

#include <common.hpp>
#include <MemoryConstants.hpp>
#include <queue>


class MemoryRegion {
public:
    MemoryRegion(u64 virtual_base, u64 size) {
        blocks.clear();
        map.clear();
        this->virtual_base = virtual_base;
        this->size = size;
    }

    u8* mem = new u8[RAM_SIZE];
    u64 virtual_base;   // Base address of this region in the virtual address space
    u64 size;

    struct Block {
        u64 start;
        size_t size;
    };
    std::vector<Block> blocks;

    struct MapEntry {
        u64 vaddr;
        u64 paddr;
        size_t size;
        u32 handle;
    };
    std::vector<MapEntry> map;

    u64 allocPhys(size_t size);
    MapEntry* alloc(size_t size);
    void free(MapEntry* entry);
    std::pair<bool, Block*> findBlockFromAddr(u64 paddr);
    std::pair<bool, Block*> findNextBlock(u64 start_addr);
    std::pair<bool, MapEntry*> findNextMappedArea(u64 start_addr);
    std::pair<bool, MapEntry*> findMapEntryWithHandle(u32 handle);
    u64 next_alloc_addr = RAM_START;
    std::pair<bool, MapEntry*> isMapped(u64 vaddr);
    MapEntry* mmap(u64 vaddr, u64 paddr, size_t size);
    void unmap(u64 vaddr);
    u64 translateAddr(u64 vaddr);
    static inline u64 pageAlign(u64 addr) { return (addr + PAGE_SIZE - 1) & ~PAGE_MASK; }

    u8* getPtrPhys(u64 paddr);
    u64 getAvailableMem();

    void printAddressMap() {
        for (auto& i : map) {
            printf("0x%016llx -> 0x%016llx\n", i.vaddr, i.vaddr + i.size - 1);
        }
    }
};

class Memory {
public:
    MemoryRegion ram = MemoryRegion(RAM_START, RAM_SIZE);
    MemoryRegion rsx = MemoryRegion(RSX_VIDEO_MEM_START, RSX_VIDEO_MEM_SIZE);
    std::vector<MemoryRegion*> regions = { &ram, &rsx };

    std::pair<u64, u8*> addrToOffsetInMemory(u64 vaddr);
    u8* getPtr(u64 vaddr);

    u64 allocPhys(size_t size) { return ram.allocPhys(size); }
    MemoryRegion::MapEntry* alloc(size_t size) { return ram.alloc(size); }
    void free(MemoryRegion::MapEntry* entry) { ram.free(entry); }
    std::pair<bool, MemoryRegion::Block*> findBlockFromAddr(u64 paddr) { return ram.findBlockFromAddr(paddr); }
    std::pair<bool, MemoryRegion::Block*> findNextBlock(u64 start_addr) { return ram.findNextBlock(start_addr); }
    std::pair<bool, MemoryRegion::MapEntry*> findNextMappedArea(u64 start_addr) { return ram.findNextMappedArea(start_addr); }
    std::pair<bool, MemoryRegion::MapEntry*> findMapEntryWithHandle(u32 handle) { return ram.findMapEntryWithHandle(handle); }
    std::pair<bool, MemoryRegion::MapEntry*> isMapped(u64 vaddr) { return ram.isMapped(vaddr); }
    MemoryRegion::MapEntry* mmap(u64 vaddr, u64 paddr, size_t size) { return ram.mmap(vaddr, paddr, size); }
    void unmap(u64 vaddr) { ram.unmap(vaddr); }
    u64 translateAddr(u64 vaddr) { return ram.translateAddr(vaddr); }

    template<typename T> T read(u64 vaddr);
    template<typename T> void write(u64 vaddr, T data);
};