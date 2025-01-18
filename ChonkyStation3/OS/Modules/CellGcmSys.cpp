#include "CellGcmSys.hpp"
#include "PlayStation3.hpp"


u64 CellGcmSys::cellGcmGetTiledPitchSize() {
    const u32 size = ARG0;
    log("cellGcmGetTiledPitchSize(size: %d) UNIMPLEMENTED\n", size);

    return Result::CELL_OK;
}

u64 CellGcmSys::cellGcmInitBody() {
    const u32 ctx_ptr = ARG0;
    const u32 cmd_size = ARG1;
    const u32 io_size = ARG2;
    const u32 io_addr = ARG3;
    log("cellGcmInitBody(ctx_ptr: 0x%08x, cmd_size: 0x%08x, io_size: 0x%08x, io_addr: 0x%08x)\n", ctx_ptr, cmd_size, io_size, io_addr);

    gcm_config.local_size = 249_MB;
    gcm_config.local_addr = ps3->mem.rsx.alloc(gcm_config.local_size)->vaddr;
    gcm_config.io_addr = io_addr;
    gcm_config.io_size = io_size;
    gcm_config.memFreq = 650000000;
    gcm_config.coreFreq = 500000000;

    std::memset(ps3->mem.getPtr(gcm_config.local_addr), 0, gcm_config.local_size);

    ctx_addr = ps3->mem.alloc(sizeof(CellGcmContextData))->vaddr;
    ctx = (CellGcmContextData*)ps3->mem.getPtr(ctx_addr);
    ps3->mem.write<u32>(ctx_ptr, ctx_addr);

    // Setup callback
    u32 callback_addr = ps3->mem.alloc(4 * 4)->vaddr;
    ps3->mem.write<u32>(callback_addr +  0, callback_addr + 4);
    ps3->mem.write<u32>(callback_addr +  4, 0x39600006);     // li r11, 0x6
    ps3->mem.write<u32>(callback_addr +  8, 0x44000002);     // sc
    ps3->mem.write<u32>(callback_addr + 12, 0x4E800020);     // blr

    ctx->begin = io_addr;
    ctx->end =  io_addr + 32 * 1024 - 4;
    ctx->current = io_addr;
    ctx->callback = callback_addr;

    dma_ctrl_addr = ps3->mem.rsx.alloc(3_MB)->vaddr;
    std::memset(ps3->mem.getPtr(dma_ctrl_addr), 0, 3_MB);
    ctrl_addr = dma_ctrl_addr + 0x40;
    ctrl = (CellGcmControl*)ps3->mem.getPtr(ctrl_addr);
    ctrl->put = 0;
    ctrl->get = 0;
    ctrl->ref = -1;
    
    // Memory watchpoint to tell the RSX to check if there are commands to run when put is written
    ps3->mem.watchpoints_w[ctrl_addr] = std::bind(&RSX::runCommandList, &ps3->rsx);
    ps3->mem.markAsSlowMem(ctrl_addr >> PAGE_SHIFT, false, true);   // Only need to make writes take the slow path

    label_addr = dma_ctrl_addr + 2_MB;

    return Result::CELL_OK;
}

u64 CellGcmSys::cellGcmAddressToOffset() {
    const u32 addr = ARG0;
    const u32 offs_ptr = ARG1;
    log("cellGcmAddressToOffset(addr: 0x%08x, offs_ptr: 0x%08x)", addr, offs_ptr);

    u32 offs = 0;
    // Check if the address is in RSX memory
    if (Helpers::inRange<u32>(addr, gcm_config.local_addr, gcm_config.local_addr + gcm_config.local_size - 1)) {
        offs = addr - gcm_config.local_addr;
    }
    // Check if it's in the IO region
    else if (Helpers::inRange<u32>(addr, gcm_config.io_addr, gcm_config.io_addr + gcm_config.io_size - 1)) {
        offs = addr - gcm_config.io_addr;
    }
    else
        Helpers::panic("\ncellGcmAddressToOffset: addr is not in rsx memory or io memory (0x%08x)\n", addr);

    logNoPrefix(" [offs: 0x%08x]\n", offs);
    ps3->mem.write<u32>(offs_ptr, offs);
    
    return Result::CELL_OK;
}

u64 CellGcmSys::cellGcmBindTile() {
    const u8 idx = ARG0;
    log("cellGcmBindTile(idx: %d) UNIMPLEMENTED\n", idx);

    return Result::CELL_OK;
}


u64 CellGcmSys::cellGcmSetFlipMode() {
    const u32 mode = ARG0;
    log("cellGcmSetFlipMode(mode: 0x%08x) UNIMPLEMENTED\n", mode);

    return Result::CELL_OK;
}

u64 CellGcmSys::cellGcmGetFlipStatus() {
    log("cellGcmGetFlipStatus()\n");

    return flip;
}

u64 CellGcmSys::cellGcmSetWaitFlip() {
    const u32 mode = ARG0;
    log("cellGcmSetWaitFlip(mode: 0x%08x) UNIMPLEMENTED\n", mode);

    return Result::CELL_OK;
}

u64 CellGcmSys::cellGcmBindZcull() {
    const u8 idx = ARG0;
    const u32 offs = ARG1;
    const u32 width = ARG2;
    const u32 height = ARG3;
    const u32 cull_start = ARG4;
    const u32 z_format = ARG5;
    const u32 aa_format = ARG6;
    const u32 z_cull_dir = ARG7;
    //const u32 s_func = ARG8;
    //const u32 s_ref = ARG9;
    //const u32 s_mask = ARG10;
    log("cellGcmBindZcull() UNIMPLEMENTED\n");

    return Result::CELL_OK;
}

u64 CellGcmSys::cellGcmMapMainMemory() {
    const u32 ea = ARG0;
    const u32 size = ARG1;
    const u32 offs_ptr = ARG2;
    log("cellGcmMapMainMemory(ea: 0x%08x, size: 0x%08x, offs_ptr: 0x%08x)\n", ea, size, offs_ptr);

    return Result::CELL_OK;
}

u64 CellGcmSys::cellGcmSetDisplayBuffer() {
    const u8 buf_id = ARG0;
    const u32 offs = ARG1;
    const u32 pitch = ARG2;
    const u32 width = ARG3;
    const u32 height = ARG4;
    log("cellGcmSetDisplayBuffer(buf_id: %d, offs: 0x%08x, pitch: %d, width: %d, height: %d)\n", buf_id, offs, pitch, width, height);

    return Result::CELL_OK;
}

u64 CellGcmSys::cellGcmGetControlRegister() {
    log("cellGcmGetControlRegister()\n");
    return ctrl_addr;
}

u64 CellGcmSys::cellGcmResetFlipStatus() {
    log("cellGcmResetFlipStatus()\n");

    flip = 1;

    return Result::CELL_OK;
}

u64 CellGcmSys::cellGcmSetTileInfo() {
    const u8 idx = ARG0;
    const u8 location = ARG1;
    const u32 offs = ARG2;
    const u32 size = ARG3;
    const u32 pitch = ARG4;
    const u8 compression = ARG5;
    const u16 base = ARG6;
    const u8 bank = ARG7;
    log("cellGcmSetTileInfo(idx: %d, location: %d, offs: 0x%08x, size: 0x%08x, pitch: 0x%08x, compression: %d, base: 0x%04x, bank: %d) UNIMPLEMENTED\n", idx, location, offs, size, pitch, compression, base, bank);

    return Result::CELL_OK;
}

u64 CellGcmSys::cellGcmSetFlip() {
    log("cellGcmSetFlip() UNIMPLEMENTED\n");
    return Result::CELL_OK;
}

u64 CellGcmSys::cellGcmGetConfiguration() {
    const u32 config_ptr = ARG0;
    log("cellGcmGetConfiguration(config_ptr: 0x%08x)\n", config_ptr);
    
    CellGcmConfig* config = (CellGcmConfig*)ps3->mem.getPtr(config_ptr);
    config->local_addr = gcm_config.local_addr;
    config->io_addr = gcm_config.io_addr;
    config->local_size = gcm_config.local_size;
    config->io_size = gcm_config.io_size;
    config->memFreq = gcm_config.memFreq;
    config->coreFreq = gcm_config.coreFreq;

    return Result::CELL_OK;
}

u64 CellGcmSys::cellGcmGetLabelAddress() {
    const u8 idx = ARG0;
    log("cellGcmGetLabelAddress(idx: 0x%02x)\n", idx);

    return label_addr + 0x10 * idx;
}


// Resets the command buffer
// If there are any remaining commands to be executed, copy them back at the start
// Update context and fifo control accordingly
u64 CellGcmSys::cellGcmCallback() {
    log("cellGcmCallback()\n");
    log("begin: 0x%08x, end: 0x%08x, current: 0x%08x\n", (u32)ctx->begin, (u32)ctx->end, (u32)ctx->current);
    log("get: 0x%08x, put: 0x%08x\n", (u32)ctrl->get, (u32)ctrl->put);

    const int bytes_executed = ctx->current - ctx->begin;
    const int bytes_remaining = bytes_executed - ctrl->put;

    if (bytes_remaining > 0)
        std::memcpy(ps3->mem.getPtr(ctx->begin), ps3->mem.getPtr(ctx->current) - bytes_remaining, bytes_remaining);

    ctx->current = ctx->begin + bytes_remaining;

    ctrl->put = bytes_remaining;
    ctrl->get = 0;
    ps3->rsx.curr_cmd = 0;

    return Result::CELL_OK;
}