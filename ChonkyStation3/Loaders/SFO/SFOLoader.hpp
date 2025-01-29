#pragma once

#include <common.hpp>
#include <logger.hpp>
#include <BEField.hpp>

#include <unordered_map>

#include <Filesystem/Filesystem.hpp>


class SFOLoader {
public:
    SFOLoader(Filesystem& fs) : fs(fs) {}
    Filesystem& fs;

    struct SFOData {
        std::unordered_map<std::string, std::u8string> strings;
        std::unordered_map<std::string, u32> ints;
    };

    SFOData parse(fs::path guest_path);

    struct SFOHeader {
        u32 magic; // Always " PSF"
        u32 version;
        u32 key_table_start;
        u32 data_table_start;
        u32 n_entries;
    };

    struct SFOIndexEntry {
        uint16_t key_offset;
        uint16_t data_fmt;
        uint32_t data_len;
        uint32_t data_max_len;
        uint32_t data_offset;
    };

private:
    MAKE_LOG_FUNCTION(log, loader_sfo);
};