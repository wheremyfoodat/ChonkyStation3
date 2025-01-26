#include "CellResc.hpp"
#include "PlayStation3.hpp"


u64 CellResc::cellRescSetDsts() {
    const u32 buf_mode = ARG0;
    const u32 dsts_ptr = ARG1;
    log("cellRescSetDsts(buf_mode: 0x%08x, dsts_ptr: 0x%08x) UNIMPLEMENTED\n", buf_mode, dsts_ptr);

    return Result::CELL_OK;
}

u64 CellResc::cellRescSetDisplayMode() {
    const u32 buf_mode = ARG0;
    log("cellRescSetDisplayMode(buf_mode: 0x%08x) UNIMPLEMENTED\n", buf_mode);

    return Result::CELL_OK;
}

u64 CellResc::cellRescSetConvertAndFlip() {
    log("cellRescSetConvertAndFlip() STUBBED\n");

    ps3->thread_manager.getCurrentThread()->sleep(1000000);

    return Result::CELL_OK;
}

u64 CellResc::cellRescInit() {
    const u32 config_ptr = ARG0;
    log("cellRescInit(config_ptr: 0x%08x) UNIMPLEMENTED\n", config_ptr);

    return Result::CELL_OK;
}

u64 CellResc::cellRescGetBufferSize() {
    const u32 color_bufs = ARG0;
    const u32 vertex_array = ARG1;
    const u32 frag_shader = ARG2;
    log("cellRescGetBufferSize(color_bufs: 0x%08x, vertex_array: 0x%08x, frag_shader: 0x%08x) UNIMPLEMENTED\n", color_bufs, vertex_array, frag_shader);

    return Result::CELL_OK;
}

u64 CellResc::cellRescSetBufferAddress() {
    const u32 color_bufs = ARG0;
    const u32 vertex_array = ARG1;
    const u32 frag_shader = ARG2;
    log("cellRescSetBufferAddress(color_bufs: 0x%08x, vertex_array: 0x%08x, frag_shader: 0x%08x) UNIMPLEMENTED\n", color_bufs, vertex_array, frag_shader);

    return Result::CELL_OK;
}

u64 CellResc::cellRescVideoOutResolutionId2RescBufferMode() {
    const u32 res_id = ARG0;
    const u32 buf_mode = ARG1;
    log("cellRescVideoOutResolutionId2RescBufferMode(res_id: %d, buf_mode: 0x%08x) UNIMPLEMENTED\n", res_id, buf_mode);

    return Result::CELL_OK;
}