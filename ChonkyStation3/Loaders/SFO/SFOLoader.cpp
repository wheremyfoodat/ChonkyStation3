#include "SFOLoader.hpp"


SFOLoader::SFOData SFOLoader::parse(fs::path guest_path) {
    SFOData sfo_data;
    const auto path = fs.guestPathToHost(guest_path);
    log("Parsing SFO %s\n", path.generic_string().c_str());

    const auto sfo = Helpers::readBinary(path);
    SFOHeader* header = (SFOHeader*)sfo.data();

    if (std::strcmp((const char*)&header->magic, "\0PSF"))
        Helpers::panic("%s is not a valid SFO file\n", path.generic_string().c_str());

    for (int i = 0; i < header->n_entries; i++) {
        SFOIndexEntry* entry = (SFOIndexEntry*)&sfo[sizeof(SFOHeader) + (sizeof(SFOIndexEntry) * i)];
        const auto key = Helpers::readString(&sfo[header->key_table_start + entry->key_offset]);
        switch (entry->data_fmt) {
        // String / Array
        case 0x004:
        case 0x204: {
            const auto data = std::u8string(reinterpret_cast<const char8_t*>(&sfo[header->data_table_start + entry->data_offset]));
            log("{ %s : %s }\n", key.c_str(), data.c_str());
            sfo_data.strings[key] = data;
            break;
        }
        // Integer
        case 0x404: {
            const u32 data = *(u32*)&sfo[header->data_table_start + entry->data_offset];
            log("{ %s : 0x%08x }\n", key.c_str(), data);
            sfo_data.ints[key] = data;
            break;
        }
        default:
            Helpers::panic("SFO: unimplemented data format type 0x%04x\n", entry->data_fmt);
        }
    }

    return sfo_data;
}