#include "CellPad.hpp"
#include "PlayStation3.hpp"


u64 CellPad::cellPadGetData() {
    const u32 port_num = ARG0;
    const u32 data_ptr = ARG1;
    log("cellPadGetData(port_num: %d, data_ptr: 0x%08x)\n");

    CellPadData* data = (CellPadData*)ps3->mem.getPtr(data_ptr);
    data->len = 8;

    for (int i = 0; i < CELL_PAD_MAX_CODES; i++)
        data->button[i] = buttons[i];

    return Result::CELL_OK;
}

u64 CellPad::cellPadGetInfo2() {
    const u32 info_ptr = ARG0;
    log("cellPadGetInfo2(info_ptr: 0x%08x)\n");

    CellPadInfo2* info = (CellPadInfo2*)ps3->mem.getPtr(info_ptr);
    info->max_connect = 4;
    info->now_connect = 1;
    info->system_info = 0;
    info->port_status[0] = 1;   // Connected
    info->port_setting[0] = 0;
    info->device_capability[0] = 1;
    info->device_type[0] = 0;   // Standard

    return Result::CELL_OK;
}