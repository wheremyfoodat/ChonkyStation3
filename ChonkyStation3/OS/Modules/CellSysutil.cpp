#include "CellSysutil.hpp"
#include "PlayStation3.hpp"


u64 CellSysutil::cellSysutilUnregisterCallback() {
    log("cellSysutilUnregisterCallback() UNIMPLEMENTED\n");

    return Result::CELL_OK;
}

u64 CellSysutil::cellSysutilCheckCallback() {
    log("cellSysutilCheckCallback() UNIMPLEMENTED\n");

    return Result::CELL_OK;
}

u64 CellSysutil::cellSysutilGetSystemParamInt() {
    const u32 param = ARG0;
    const u32 val_ptr = ARG1;
    log("cellSysutilGetSystemParamInt(param: 0x%08x, val_ptr: 0x%08x)\n", param, val_ptr);

    u32 val = 0;
    switch (param) {
    case SYSTEM_PARAM::CELL_SYSUTIL_SYSTEMPARAM_ID_LANG:                val = SYSTEM_LANG::CELL_SYSUTIL_LANG_ENGLISH_US;            break;
    case SYSTEM_PARAM::CELL_SYSUTIL_SYSTEMPARAM_ID_ENTER_BUTTON_ASSIGN: val = ENTER_BUTTON::CELL_SYSUTIL_ENTER_BUTTON_ASSIGN_CROSS; break;

    default:
        Helpers::panic("Tried to get unimplemented system param 0x%08x\n", param);
    }

    ps3->mem.write<u32>(val_ptr, val);
    return Result::CELL_OK;
}

u64 CellSysutil::cellSysutilRegisterCallback() {
    log("cellSysUtilRegisterCallback() UNIMPLEMENTED\n");

    return Result::CELL_OK;
}