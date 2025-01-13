#include "CellResc.hpp"
#include "PlayStation3.hpp"


u64 CellResc::cellRescInit() {
    const u32 config_ptr = ARG0;
    log("cellRescInit(config_ptr: 0x%08x) UNIMPLEMENTED\n", config_ptr);

    return Result::CELL_OK;
}

u64 CellResc::cellRescVideoOutResolutionId2RescBufferMode() {
    const u32 res_id = ARG0;
    const u32 buf_mode = ARG1;
    log("cellRescVideoOutResolutionId2RescBufferMode(res_id: %d, buf_mode: 0x%08x) UNIMPLEMENTED\n", res_id, buf_mode);

    return Result::CELL_OK;
}