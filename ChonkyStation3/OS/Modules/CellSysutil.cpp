#include "CellSysutil.hpp"
#include "PlayStation3.hpp"


u64 CellSysutil::cellSysutilUnregisterCallback() {
    log("cellSysutilUnregisterCallback() UNIMPLEMENTED\n");

    return Result::CELL_OK;
}

u64 CellSysutil::cellSysutilCheckCallback() {
    log("cellSysutilCheckCallback() STUBBED\n");

    // TODO: Currently this is stubbed to just always call the callbacks.
    for (int i = 0; i < 3; i++) {
        if (callbacks[i].func_ptr) {
            log("Running callback func %d\n", i);
            ps3->ppu->state.gprs[3] = callbacks[i].userdata_ptr;
            ps3->ppu->runFunc(callbacks[i].func_ptr);
            log("Done\n");
        }
    }

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
    const u32 slot = ARG0;
    const u32 func_ptr = ARG1;
    const u32 userdata_ptr = ARG2;
    log("cellSysUtilRegisterCallback(slot: %d, func_ptr: 0x%08x, userdata_ptr: 0x%08x)\n", slot, func_ptr, userdata_ptr);
    Helpers::debugAssert(slot < 4, "cellSysutilRegisterCallback(): slot is not in range 0-3\n");

    callbacks[slot] = { ps3->mem.read<u32>(func_ptr), userdata_ptr };

    return Result::CELL_OK;
}