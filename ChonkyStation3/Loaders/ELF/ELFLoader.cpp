#include "ELFLoader.hpp"


using namespace ELFIO;

u64 ELFLoader::load(const fs::path& path, std::unordered_map<u32, u32>& imports, ModuleManager& module_manager) {
    elfio elf;

    auto str = path.generic_string();
    if (!elf.load(str.c_str())) {
        Helpers::panic("Couldn't load ELF {}", path);
    }

    printf("Loading ELF %s\n", str.c_str());
    printf("* %d segments\n", elf.segments.size());
    for (int i = 0; i < elf.segments.size(); i++) {
        auto seg = elf.segments[i];
        // Skip the segment if it's empty
        if (seg->get_memory_size() == 0) {
            printf("* Segment %d type %s: empty\n", i, segment_type_string[seg->get_type()].c_str());
            continue;
        }

        printf("* Segment %d type %s: 0x%016llx -> 0x%016llx\n", i, segment_type_string[seg->get_type()].c_str(), seg->get_virtual_address(), seg->get_virtual_address() + seg->get_memory_size());
        
        // PT_TLS
        if (seg->get_type() == PT_TLS) {
            tls_vaddr = seg->get_virtual_address();
            tls_filesize = seg->get_file_size();
            tls_memsize = seg->get_memory_size();
        }

        // PROC_PARAM
        else if (seg->get_type() == PROC_PARAM) {
        }
        
        // PRX_PARAM
        else if (seg->get_type() == PRX_PARAM) {
            // Check prx magic
            PrxParam* prx_param = (PrxParam*)seg->get_data();
            if (prx_param->magic != PRX_MAGIC) {
                Helpers::panic("PRX PARAM wrong magic (0x%08x)", prx_param->magic);
            }

            printf("size         : 0x%08x\n", (u32)prx_param->size);
            printf("magic        : 0x%08x\n", (u32)prx_param->magic);
            printf("version      : 0x%08x\n", (u32)prx_param->version);
            printf("libentstart  : 0x%08x\n", (u32)prx_param->libentstart);
            printf("libstubstart : 0x%08x\n", (u32)prx_param->libstubstart);
            printf("libstubend   : 0x%08x\n", (u32)prx_param->libstubend);
            printf("ver          : 0x%08x\n", (u16)prx_param->ver);

            // Stubs
            for (int i = prx_param->libstubstart; i < prx_param->libstubend; i += sizeof(PrxStubHeader)) {
                u64 ptr = mem.translateAddr(i);
                PrxStubHeader* stub = (PrxStubHeader*)&mem.ram[ptr];

                /*printf("Found stub\n");
                printf("s_modulename : 0x%016x\n", (u32)stub->s_modulename);
                printf("s_imports    : %d\n", (u32)stub->s_imports);*/

                if (stub->s_modulename == 0) {
                    printf("Skipping null module\n");
                    continue;
                }

                std::string name;
                u8* namePtr = mem.getPtr(stub->s_modulename);
                name = Helpers::readString(namePtr);
                printf("\nFound module %s with %d imports\n", name.c_str(), (u16)stub->s_imports);
                for (int i = 0; i < stub->s_imports; i++) {
                    u32 nid = mem.read<u32>(stub->s_nid + i * 4);
                    u32 addr = mem.read<u32>(stub->s_text + i * 4);
                    imports[addr] = nid;
                    if (module_manager.import_map.find(nid) != module_manager.import_map.end())
                        printf("* %s			@ 0x%08x\n", module_manager.import_map[nid].name.c_str(), addr);
                    else
                        printf("* unk_0x%08x			@ 0x%08x\n", nid, addr);

                    // Patch import stub
                    // Find BCCTR or BCCTRL instructions
                    // There are 2 different kinds of stubs
                    bool stubbed = false;
                    for (int i = 0; i < 128; i += 4) {
                        const auto instr = mem.read<u32>(addr + i);
                        if (instr == 0x4e800420) {
                            mem.write<u32>(addr + i - 4, 0x39600010);   // li r11, 0x10
                            mem.write<u32>(addr + i - 0, 0x44000000);   // sc
                            stubbed = true;
                            break;
                        }
                        else if (instr == 0x4e800421) {
                            mem.write<u32>(addr + i - 4, 0x39600011);   // li r11, 0x11
                            mem.write<u32>(addr + i - 0, 0x44000000);   // sc
                            stubbed = true;
                            break;
                        }
                    }
                    if (!stubbed) Helpers::panic("Couldn't patch stub\n");
                }
            }
            putc('\n', stdout);
        }

        // Load segment only if it's of type PT_LOAD
        if (seg->get_type() == PT_LOAD) {
            u64 size = seg->get_memory_size();
            u64 paddr = mem.alloc(size);
            mem.mmap(seg->get_virtual_address(), paddr, size);

            std::memcpy(mem.getPtr(seg->get_virtual_address()), seg->get_data(), seg->get_file_size());
            // Set the remaining memory to 0
            std::memset(mem.getPtr(seg->get_virtual_address()) + seg->get_file_size(), 0, seg->get_memory_size() - seg->get_file_size());
        }
    }

    // The entry point in the ELF header is actually an address from which you read the actual entry point
    u64 entry = mem.read<u32>(elf.get_entry());
    printf("* Entry point: 0x%016llx (0x%016llx)\n", elf.get_entry(), entry);
    // But we return the address from the ELF header (need it, R2 is loaded to entry + 4 on load)
    return elf.get_entry();
}