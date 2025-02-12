#include "CellPad.hpp"
#include "PlayStation3.hpp"


u64 CellPad::cellPadInit() {
    const u32 max_connect = ARG0;
    log("cellPadInit(max_connect: %d)\n", max_connect);

    return Result::CELL_OK;
}

u64 CellPad::cellPadGetData() {
    const u32 port_num = ARG0;
    const u32 data_ptr = ARG1;
    log("cellPadGetData(port_num: %d, data_ptr: 0x%08x)\n");

    CellPadData* data = (CellPadData*)ps3->mem.getPtr(data_ptr);
    data->len = 8;

    for (int i = 0; i < CELL_PAD_MAX_CODES; i++)
        data->button[i] = buttons[i];
    data->button[1] = (data->len / 2) & 0xf;
    data->button[4] = 0x7f; // Right stick X
    data->button[5] = 0x7f; // Right stick Y
    data->button[6] = 0x7f; // Left stick X
    data->button[7] = 0x7f; // Left stick Y

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
    //ps3->ppu->should_log = true;

    return Result::CELL_OK;
}