#include "Memory.hpp"


// Allocates size bytes of physical memory. Returns physical address of the allocated memory.
u64 Memory::allocPhys(size_t size) {
    // Page alignment
    size_t aligned_size = pageAlign(size);
    // Find the next free block of memory big enough for the given size
    u64 addr = 0;
    while (true) {
        auto next_block = findNextBlock(addr);
        if (next_block.first) {
            if (next_block.second->start - addr >= aligned_size) {
                // addr OK
                break;
            }
            else {
                // Keep searching
                addr = next_block.second->start + next_block.second->size;
                if (addr >= RAM_SIZE)
                    Helpers::panic("Out of memory\n");
            }
        }
        else {
            // addr OK
            break;
        }
    }

    // Did we run out of memory?
    if (addr + aligned_size >= RAM_SIZE)
        Helpers::panic("Out of memory\n");

    // Allocate block
    blocks.push_back({ addr, aligned_size });

    return addr;
}

// Allocates and maps size bytes of memory. Returns virtual address of allocated memory.
Memory::MapEntry* Memory::alloc(size_t size) {
    // Page alignment
    size_t aligned_size = pageAlign(size);
    // Allocate block of memory
    u64 paddr = allocPhys(aligned_size);
    u64 vaddr = RAM_START;
    // Find the next free area in the address map
    printf("Searching allocateable area...\n");
    while (true) {
        auto next_area = findNextMappedArea(vaddr);
        printf("0x%016llx (next area at 0x%016llx, spacing: 0x%016llx)...", vaddr, next_area.second->vaddr, next_area.second->vaddr - vaddr);
        if (next_area.first) {
            if (next_area.second->vaddr - vaddr >= aligned_size) {
                // addr OK
                break;
            }
            else {
                // Keep searching
                vaddr = next_area.second->vaddr + next_area.second->size;
                printf(" not ok\n");
            }
        }
        else {
            // addr OK
            break;
        }
    }
    printf(" ok\n");

    // Map area
    MapEntry* entry = mmap(vaddr, paddr, aligned_size);

    printf("Allocated 0x%08llx bytes at 0x%016llx\n", aligned_size, vaddr);
    return entry;
}

// Returns whether there is an allocated memory block after the given physical address and, in case there is, returns the block info.
std::pair<bool, Memory::Block*> Memory::findNextBlock(u64 start_addr) {
    Block* block = nullptr;
    for (auto& i : blocks) {
        if (i.start >= start_addr) {
            if (block == nullptr)
                block = &i;
            else if (i.start < block->start)
                block = &i;
        }
    }

    return { block != nullptr, block };
}

// Returns whether there is a mapped area in the virtual address space after the given virtual address and, in case there is, returns the map info.
std::pair<bool, Memory::MapEntry*> Memory::findNextMappedArea(u64 start_addr) {
    MapEntry* map_entry = nullptr;
    for (auto& i : map) {
        if (i.vaddr >= start_addr) {
            if (map_entry == nullptr)
                map_entry = &i;
            else if (i.vaddr < map_entry->vaddr)
                map_entry = &i;
        }
    }

    return { map_entry != nullptr, map_entry };
}

// Returns whether there is a mapped area in the virtual address space with the given handle and, in case there is, returns the map info.
std::pair<bool, Memory::MapEntry*> Memory::findMapEntryWithHandle(u32 handle) {
    for (auto& i : map) {
        if (i.handle == handle)
            return { true, &i };
    }
    return { false, nullptr };
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
Memory::MapEntry* Memory::mmap(u64 vaddr, u64 paddr, size_t size) {
    if (isMapped(vaddr).first) {
        printAddressMap();
        Helpers::panic("Tried to map an already mapped virtual address at 0x%016llx\n", vaddr);
    }

    // Page alignment
    size_t aligned_size = pageAlign(size);

    map.push_back({ vaddr, paddr, aligned_size });
    return &map.back();
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

// Returns a pointer to the data at the specified virtual address
u8* Memory::getPtr(u64 vaddr) {
    u64 paddr = translateAddr(vaddr);
    return &ram[paddr & (256_MB) - 1];
}

// Returns a pointer to the data at the specified physical address
u8* Memory::getPtrPhys(u64 paddr) {
    return &ram[paddr & (256_MB) - 1];
}

// Returns amount of available memory
u64 Memory::getAvailableMem() {
    return RAM_END - next_alloc_addr;
}

template<typename T>
T Memory::read(u64 vaddr) {
    u64 paddr = translateAddr(vaddr);
    T data;

    std::memcpy(&data, &ram[paddr & (256_MB - 1)], sizeof(T));

    return Helpers::bswap<T>(data);
}
template u8  Memory::read(u64 vaddr);
template u16 Memory::read(u64 vaddr);
template u32 Memory::read(u64 vaddr);
template u64 Memory::read(u64 vaddr);

template<typename T>
void Memory::write(u64 vaddr, T data) {
    u64 paddr = translateAddr(vaddr);
    data = Helpers::bswap<T>(data);

    std::memcpy(&ram[paddr & (256_MB - 1)], &data, sizeof(T));
}
template void Memory::write(u64 vaddr, u8  data);
template void Memory::write(u64 vaddr, u16 data);
template void Memory::write(u64 vaddr, u32 data);
template void Memory::write(u64 vaddr, u64 data);