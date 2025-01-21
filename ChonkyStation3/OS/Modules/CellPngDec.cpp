#include "CellPngDec.hpp"
#include "PlayStation3.hpp"


u64 CellPngDec::cellPngDecDecodeData() {
    log("cellPngDecDecodeData()\n");
    return Result::CELL_OK;
}

u64 CellPngDec::cellPngDecClose() {
    log("cellPngDecClose()\n");
    return Result::CELL_OK;
}

u64 CellPngDec::cellPngDecDestroy() {
    log("cellPngDecDestroy()\n");
    return Result::CELL_OK;
}

u64 CellPngDec::cellPngDecReadHeader() {
    log("cellPngDecReadHeader()\n");
    return Result::CELL_OK;
}

u64 CellPngDec::cellPngDecCreate() {
    const u32 handle_ptr = ARG0;
    const u32 thread_in_param_ptr = ARG1;
    const u32 thread_out_param_ptr = ARG2;

    ps3->mem.write<u32>(handle_ptr, ps3->handle_manager.request());

    log("cellPngDecCreate(handle_ptr: 0x%08x, thread_in_param_ptr: 0x%08x, thread_out_param_ptr: 0x%08x)\n", handle_ptr, thread_in_param_ptr, thread_out_param_ptr);
    return Result::CELL_OK;
}

u64 CellPngDec::cellPngDecOpen() {
    const u32 handle = ARG0;
    const u32 subhandle_ptr = ARG1;
    const u32 src_ptr = ARG2;
    const u32 open_info_ptr = ARG3;
    log("cellPngDecOpen(handle: 0x%08x, subhandle_ptr: 0x%08x, src_ptr: 0x%08x, open_info_ptr: 0x%08x)\n", handle, subhandle_ptr, src_ptr, open_info_ptr);

    CellPngDecSrc* src = (CellPngDecSrc*)ps3->mem.getPtr(src_ptr);
    curr_src.src_sel = src->src_sel;
    if (src->filename_ptr)
        curr_src.filename = ps3->mem.getPtr(src->filename_ptr);
    curr_src.file_offs = src->file_offs;
    curr_src.file_size = src->file_size;
    if (src->stream_ptr)
        curr_src.stream = ps3->mem.getPtr(src->stream_ptr);
    curr_src.stream_size = src->stream_size;
    curr_src.spu_thread_enable = src->spu_thread_enable;

    log("Source type: %s\n", curr_src.src_sel == CellPngDecStreamSrcSel::CELL_PNGDEC_BUFFER ? "BUFFER" : "FILE");

    return Result::CELL_OK;
}

u64 CellPngDec::cellPngDecSetParameter() {
    log("cellPngDecSetParameter()\n");
    return Result::CELL_OK;
}