#pragma once

#include <common.hpp>
#include <logger.hpp>
#include <BEField.hpp>
#include <elfio/elfio.hpp>

#include <unordered_map>


// Circular dependency
class PlayStation3;

static constexpr size_t EHDR_SIZE = 42;

static constexpr s32 SYS_SPU_SEGMENT_TYPE_COPY = 0x0001;
static constexpr s32 SYS_SPU_SEGMENT_TYPE_FILL = 0x0002;
static constexpr s32 SYS_SPU_SEGMENT_TYPE_INFO = 0x0004;

class SPULoader {
public:
    SPULoader(PlayStation3* ps3) : ps3(ps3) {}
    PlayStation3* ps3;

    // We break our naming conventions here to follow CellOS's instead
    struct sys_spu_segment {
        BEField<s32> type;       // Segment type (COPY / FILL / INFO)
        BEField<u32> ls_addr;    // Addr in LS that this segment will be loaded to
        BEField<s32> size;
        union {
            BEField<u32> addr;   // Address of the segment in memory, for COPY type segments
            BEField<u32> val;    // The value to fill the segment with, for FILL type segments
            BEField<u64> pad;
        } src;
    };

    struct sys_spu_image {
        BEField<u32> type;
        BEField<u32> entry;
        BEField<u32> segs_ptr;  // segs is sys_spu_segment[n_segs]
        BEField<u32> n_segs;
    };

    void load(u32 img_ptr, sys_spu_image* img);

private:
    MAKE_LOG_FUNCTION(log, loader_spu);
};