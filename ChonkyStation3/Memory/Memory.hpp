#pragma once

#include <common.hpp>
#include <MemoryConstants.hpp>
#include <queue>


class Memory {
public:
    Memory() {
        blocks.clear();
        map.clear();
    }

    u8* ram = new u8[RAM_SIZE];

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
    std::pair<bool, Block*> findNextBlock(u64 start_addr);
    std::pair<bool, MapEntry*> findNextMappedArea(u64 start_addr);
    std::pair<bool, MapEntry*> findMapEntryWithHandle(u32 handle);
    u64 next_alloc_addr = RAM_START;
    std::pair<bool, MapEntry*> isMapped(u64 vaddr);
    MapEntry* mmap(u64 vaddr, u64 paddr, size_t size);
    void unmap(u64 vaddr);
    u64 translateAddr(u64 vaddr);
    u8* getPtr(u64 vaddr);
    u8* getPtrPhys(u64 paddr);
    static inline u64 pageAlign(u64 addr) { return (addr + PAGE_SIZE - 1) & ~PAGE_MASK; }

    u64 getAvailableMem();

    template<typename T> T read(u64 vaddr);
    template<typename T> void write(u64 vaddr, T data);

    void printAddressMap() {
        for (auto& i : map) {
            printf("0x%016llx -> 0x%016llx\n", i.vaddr, i.vaddr + i.size - 1);
        }
    }
};