#include "CellRtc.hpp"
#include "PlayStation3.hpp"


u64 CellRtc::cellRtcGetCurrentTick() {
    const u32 tick_ptr = ARG0;
    log("cellRtcGetCurrentTick(tick_ptr: 0x%08x)\n", tick_ptr);

    ps3->mem.write<u64>(tick_ptr, ps3->scheduler.time);
    
    return Result::CELL_OK;
}