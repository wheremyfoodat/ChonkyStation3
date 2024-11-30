#include "ELFLoader.hpp"


using namespace ELFIO;

void ELFLoader::load(const fs::path& path) {
	elfio elf;

	auto str = path.generic_string();
	if (!elf.load(str.c_str())) {
		Helpers::panic("Couldn't load ELF {}", path);
	}

	printf("Loading ELF %s\n", str.c_str());
	printf("* Entry point: 0x%016llx\n", elf.get_entry());
	printf("* %d segments\n", elf.segments.size());
	for (int i = 0; i < elf.segments.size(); i++) {
		auto seg = elf.segments[i];
		// Skip the segment if it's empty
		if (seg->get_file_size() == 0) {
			printf("* Segment %d type %s: empty\n", i, segment_type_string[seg->get_type()].c_str());
			continue;
		}

		if (seg->get_type() != PROC_PARAM && seg->get_type() != PRX_PARAM)
			printf("* Segment %d type %s: 0x%016llx -> 0x%016llx\n", i, segment_type_string[seg->get_type()].c_str(), seg->get_virtual_address(), seg->get_virtual_address() + seg->get_memory_size());
		// PROC_PARAM
		else if (seg->get_type() == PROC_PARAM) {
			printf("* Segment %d type %s: magic 0x%08x\n", i, segment_type_string[seg->get_type()].c_str(), 0);
		}
		// PRX_PARAM
		else if (seg->get_type() == PRX_PARAM) {
			// Check prx magic
			PrxParam* prx_param = (PrxParam*)seg->get_data();
			if (prx_param->magic != PRX_MAGIC) {
				Helpers::panic("PRX PARAM wrong magic (0x%08x)", prx_param->magic);
			}
			printf("* Segment %d type %s\n", i, segment_type_string[seg->get_type()].c_str());
			printf("size         : 0x%08x\n", (u32)prx_param->size);
			printf("magic        : 0x%08x\n", (u32)prx_param->magic);
			printf("version      : 0x%08x\n", (u32)prx_param->version);
			printf("libentstart  : 0x%08x\n", (u32)prx_param->libentstart);
			printf("libstubstart : 0x%08x\n", (u32)prx_param->libstubstart);
			printf("libstubend   : 0x%08x\n", (u32)prx_param->libstubend);
			printf("ver          : 0x%08x\n", (u16)prx_param->ver);

			// Stubs
			for (int i = prx_param->libstubstart; i <= prx_param->libstubend; i += sizeof(PrxStubHeader)) {
				u64 ptr = mem.translateAddr(i);
				PrxStubHeader* stub = (PrxStubHeader*)&mem.ram[ptr];

				printf("Found stub\n");
				printf("s_modulename : 0x%016x\n", (u32)stub->s_modulename);
				printf("s_imports    : %d\n", (u32)stub->s_imports);

				std::string name;
				u64 nameAddr = mem.translateAddr(stub->s_modulename);
				while (mem.ram[nameAddr] != '\0')
					name += mem.ram[nameAddr++];
				printf("Found module %s with %d imports\n", name.c_str(), (u16)stub->s_imports);
			}
		}

		// Load segment only if it's of type PT_LOAD
		if (seg->get_type() == PT_LOAD) {
			u64 size = seg->get_memory_size();
			u64 paddr = mem.alloc(size);
			mem.mmap(seg->get_virtual_address(), paddr, size);
			memcpy(&mem.ram[mem.translateAddr(seg->get_virtual_address()) & (256_MB - 1)], seg->get_data(), seg->get_file_size());
		}
	}

}