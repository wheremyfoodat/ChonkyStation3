#pragma once

#include <common.hpp>
#include <BEField.hpp>
#include <elfio/elfio.hpp>
#include <logger.hpp>
#include <Memory.hpp>
#include <ModuleManager.hpp>
#include <unordered_map>


static constexpr u32 SCE_PPURELA = 0x700000a4;

class PRXLoader {
public:
    PRXLoader(Memory& mem) : mem(mem) {}
    Memory& mem;
    void load(const fs::path& path, std::unordered_map<u32, u32>& imports);

    std::unordered_map<u64, std::string> segment_type_string {
        { ELFIO::PT_LOAD,   "PT_LOAD    " },
        { ELFIO::PT_TLS,    "PT_TLS     " },
        { SCE_PPURELA,      "SCE_PPURELA" },
    };

    struct PRXRelocation {
        BEField<u64> offs;
        BEField<u16> unk;
        u8 data_idx;    // Segment index for relocated data
        u8 addr_idx;    // Segment index for relocated address (to write the data to)
        BEField<u32> type;
        BEField<u64> ptr;
    };

private:
    MAKE_LOG_FUNCTION(log, loader_prx);
};