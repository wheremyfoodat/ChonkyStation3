#pragma once

#include <common.hpp>
#include <logger.hpp>
#include <BEField.hpp>

#include <CellTypes.hpp>


// Circular dependency
class PlayStation3;

using namespace CellTypes;

class CellPngDec {
public:
    CellPngDec(PlayStation3* ps3) : ps3(ps3) {}
    PlayStation3* ps3;

    struct CellPngDecSrc {
        BEField<u32> src_sel;
        u32 filename_ptr;
        BEField<s64> file_offs;
        BEField<u32> file_size;
        u32 stream_ptr;
        BEField<u32> stream_size;
        BEField<u32> spu_thread_enable;
    };

    struct CellPngDecSrcLE {
        u32 src_sel;
        u8* filename;
        s64 file_offs;
        u32 file_size;
        u8* stream;
        u32 stream_size;
        u32 spu_thread_enable;
    };

    enum CellPngDecStreamSrcSel : u32 {
        CELL_PNGDEC_FILE = 0,
        CELL_PNGDEC_BUFFER = 1,
    };

    CellPngDecSrcLE curr_src;

    u64 cellPngDecDecodeData();
    u64 cellPngDecClose();
    u64 cellPngDecDestroy();
    u64 cellPngDecReadHeader();
    u64 cellPngDecCreate();
    u64 cellPngDecOpen();
    u64 cellPngDecSetParameter();

private:
    MAKE_LOG_FUNCTION(log, cellPngDec);
};