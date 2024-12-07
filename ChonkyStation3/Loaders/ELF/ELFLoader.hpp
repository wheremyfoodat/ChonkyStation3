#pragma once

#include <common.hpp>
#include <BEField.hpp>
#include <elfio/elfio.hpp>
#include <Memory.hpp>
#include <map>


//constexpr u32 PT_LOAD = 0x00000001; // Already defined in ELFIO
//constexpr u32 PT_TLS = 0x00000007; // Already defined in ELFIO
// PS3 specific
constexpr u32 PROC_PARAM = 0x60000001;
constexpr u32 PRX_PARAM  = 0x60000002;

constexpr u32 PROC_MAGIC = 0x13bcc5f6;
constexpr u32 PRX_MAGIC  = 0x1b434cec;

class ELFLoader {
public:
    ELFLoader(Memory& mem) : mem(mem) {}
    Memory& mem;
    u64 load(const fs::path& path);

    std::map<u64, std::string> segment_type_string {
        { ELFIO::PT_LOAD,   "PT_LOAD    " },
        { ELFIO::PT_TLS,    "PT_TLS     " },
        { PROC_PARAM,       "PROC_PARAM " },
        { PRX_PARAM,        "PRX_PARAM  " },
    };

    struct PrxParam {
        BEField<u32> size;
        BEField<u32> magic;
        BEField<u32> version;
        BEField<u32> pad0;
        BEField<u32> libentstart;
        BEField<u32> libentend;
        BEField<u32> libstubstart;
        BEField<u32> libstubend;
        BEField<u16> ver;
        BEField<u16> pad1;
        BEField<u32> pad2;
    };

    struct PrxStubHeader {
        u8 s_size;
        u8 s_unk0;
        BEField<u16> s_version;
        BEField<u16> s_unk1;
        BEField<u16> s_imports;
        BEField<u32> s_unk2;
        BEField<u32> s_unk3;
        BEField<u32> s_modulename;
        BEField<u32> s_nid;
        BEField<u32> s_text;
        BEField<u32> s_unk4;
        BEField<u32> s_unk5;
        BEField<u32> s_unk6;
        BEField<u32> s_unk7;
    };
};