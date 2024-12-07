#pragma once

#include <common.hpp>


constexpr u64 PAGE_SIZE = 0x1000;
constexpr u64 PAGE_MASK = PAGE_SIZE - 1;
constexpr u64 RAM_START = 0x00010000;
constexpr u64 RAM_END = RAM_START + 256_MB;
constexpr u64 STACK_REGION_START = 0xD0000000;
constexpr u64 STACK_REGION_SIZE = 0x10000000;
constexpr u64 DEFAULT_STACK_SIZE = 0x10000;

class Memory {
public:
	Memory() {}

	u8* ram = new u8[256_MB];

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

	template<typename T> T read(u64 vaddr);
	template<typename T> void write(u64 vaddr, T data);
};