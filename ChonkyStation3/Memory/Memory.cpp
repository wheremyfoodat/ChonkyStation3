#include "Memory.hpp"


// Allocates size bytes. Returns physical address of the allocated memory.
u64 Memory::alloc(size_t size) {
	// Page alignment
	size_t aligned_size = (size + PAGE_SIZE - 1) & ~PAGE_MASK;
	// Allocate block
	u64 block_start = next_alloc_addr;
	blocks.push_back({ block_start, aligned_size });
	// Increment address of next block
	next_alloc_addr += aligned_size;

	return block_start;
}

// Checks if the given virtual address is mapped to a physical address.
// If it is, also return the map entry.
std::pair<bool, Memory::MapEntry*> Memory::isMapped(u64 vaddr) {
	for (auto& i : map) {
		if (Helpers::inRange<u32>(vaddr, i.vaddr, i.vaddr + i.size - 1)) return { true, &i };
	}

	return { false, nullptr };
}

// Maps a virtual address to a physical one.
void Memory::mmap(u64 vaddr, u64 paddr, size_t size) {
	if (isMapped(vaddr).first) Helpers::panic("Tried to map an already mapped virtual address");

	// Page alignment
	size_t aligned_size = (size + PAGE_SIZE - 1) & ~PAGE_MASK;

	map.push_back({ vaddr, paddr, aligned_size });
}

// Unmaps the region starting at the given virtual address
void Memory::unmap(u64 vaddr) {
	for (int i = 0; i < map.size(); i++) {
		if (map[i].vaddr == vaddr) {
			map.erase(map.begin() + i);
		}
	}
}

// Translates a virtual address
u64 Memory::translateAddr(u64 vaddr) {
	auto [mapped, map] = isMapped(vaddr);
	if (!mapped) Helpers::panic("Tried to access unmapped vaddr 0x%016x\n", vaddr);

	return map->paddr + (vaddr - map->vaddr);
}