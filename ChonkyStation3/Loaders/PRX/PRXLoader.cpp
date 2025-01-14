#include "PRXLoader.hpp"


using namespace ELFIO;

void PRXLoader::load(const fs::path& path, std::unordered_map<u32, u32>& imports) {
    const auto elf_binary = Helpers::readBinary(path);
    elfio elf;

    // TODO: SPRX (encrypted)
    if (elf_binary[0] == 'S' && elf_binary[1] == 'C' && elf_binary[2] == 'E') {
        Helpers::panic("PRXLoader: PRX is encrypted (%s)\n", path.c_str());
    }

    auto str = path.generic_string();
    if (!elf.load(str.c_str())) {
        Helpers::panic("Couldn't load PRX %s", path.generic_string().c_str());
    }

    log("Loading PRX %s\n", path.filename().generic_string().c_str());
    log("* %d segments\n", elf.segments.size());
    // Allocate segments
    std::vector<MemoryRegion::MapEntry*> allocations;
    for (int i = 0; i < elf.segments.size(); i++) {
        auto seg = elf.segments[i];
        // Skip the segment if it's empty
        if (seg->get_memory_size() == 0) {
            log("* Segment %d type %s: empty\n", i, segment_type_string[seg->get_type()].c_str());
            continue;
        }
        log("* Segment %d type %s: 0x%016llx -> 0x%016llx\n", i, segment_type_string[seg->get_type()].c_str(), seg->get_virtual_address(), seg->get_virtual_address() + seg->get_memory_size());

        if (seg->get_type() == PT_LOAD) {
            // Allocate segment in memory
            auto entry = mem.alloc(seg->get_memory_size());
            allocations.push_back(entry);
            u8* ptr = mem.ram.getPtrPhys(entry->paddr);
            mem.markAsFastMem(entry->vaddr >> PAGE_SHIFT, ptr, true, true);
            std::memcpy(ptr, seg->get_data(), seg->get_file_size());
            // Set the remaining memory to 0
            std::memset(ptr + seg->get_file_size(), 0, seg->get_memory_size() - seg->get_file_size());
        }
    }

    // Do relocations
    for (int i = 0; i < elf.segments.size(); i++) {
        auto seg = elf.segments[i];

        if (seg->get_type() == SCE_PPURELA) {
            for (int i = 0; i < seg->get_file_size(); i += sizeof(PRXRelocation)) {
                PRXRelocation* reloc = (PRXRelocation*)&seg->get_data()[i];
                //log("Relocation: offs: 0x%08x, addr_idx: %d, data_idx: %d, type: %d, ptr: 0x%016llx\n", (u64)reloc->offs, (u8)reloc->addr_idx, (u8)reloc->data_idx, (u32)reloc->type, (u64)reloc->ptr);
                Helpers::debugAssert(reloc->addr_idx < elf.segments.size(), "PRXLoader: addr idx is out of range\n");
                Helpers::debugAssert(reloc->data_idx < elf.segments.size(), "PRXLoader: data idx is out of range\n");

                const u32 base = allocations[reloc->data_idx]->vaddr;
                const u32 data = base + reloc->ptr;
                const u32 reloc_addr = allocations[reloc->addr_idx]->vaddr + reloc->offs;

                log("Relocated address: ");
                switch (reloc->type) {
                case 1: {
                    mem.write<u32>(reloc_addr, data);
                    logNoPrefix("[0x%08x] <- 0x%08x\n", reloc_addr, data);
                    break;
                }

                case 4: {
                    mem.write<u16>(reloc_addr, data);
                    logNoPrefix("[0x%08x] <- 0x%04x\n", reloc_addr, data);
                    break;
                }

                case 6: {
                    const u16 reloc_data = (u16)(data >> 16) + ((data >> 15) & 1);
                    mem.write<u16>(reloc_addr, reloc_data);
                    logNoPrefix("[0x%08x] <- 0x%04x\n", reloc_addr, reloc_data);
                    break;
                }

                default:
                    Helpers::panic("PRXLoader: unimplemented relocation type %d\n", (u32)reloc->type);
                }
            }
        }
    }
}