#pragma once

#include <common.hpp>
#include <MemoryConstants.hpp>


class Memory {
public:
    Memory() {}

    u8* ram = new u8[RAM_SIZE];

    struct Block {
        u64 start;
        size_t size;
    };
    std::vector<Block> blocks;
    u64 alloc(size_t size);
    u64 next_alloc_addr = RAM_START;

    struct MapEntry {
        u64 vaddr;
        u64 paddr;
        size_t size;
    };
    std::vector<MapEntry> map;
    std::pair<bool, MapEntry*> isMapped(u64 vaddr);
    void mmap(u64 vaddr, u64 paddr, size_t size);
    void unmap(u64 vaddr);
    u64 translateAddr(u64 vaddr);
    u8* getPtr(u64 vaddr);
    u8* getPtrPhys(u64 paddr);

    u64 getAvailableMem();

    template<typename T> T read(u64 vaddr);
    template<typename T> void write(u64 vaddr, T data);
};