#include "CellGcmSys.hpp"
#include "PlayStation3.hpp"


u64 CellGcmSys::cellGcmInitBody() {
    const u32 ctx_ptr = ARG0;
    const u32 cmd_size = ARG1;
    const u32 io_size = ARG2;
    const u32 io_addr = ARG3;
    printf("cellGcmInitBody(ctx_ptr: 0x%08x, cmd_size: 0x%08x, io_size: 0x%08x, io_addr: 0x%08x)\n", ctx_ptr, cmd_size, io_size, io_addr);

    gcm_config.local_size = 249_MB;
    gcm_config.local_addr = ps3->mem.rsx.alloc(gcm_config.local_size)->vaddr;
    gcm_config.io_addr = io_addr;
    gcm_config.io_size = io_size;
    gcm_config.memFreq = 650000000;
    gcm_config.coreFreq = 500000000;

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
    ctrl_addr = dma_ctrl_addr + 0x40;
    ctrl = (CellGcmControl*)ps3->mem.getPtr(ctrl_addr);
    ctrl->put = 0;
    ctrl->get = 0;
    ctrl->ref = -1;
    
    label_addr = dma_ctrl_addr + 2_MB;

    return Result::CELL_OK;
}

u64 CellGcmSys::cellGcmAddressToOffset() {
    const u32 addr = ARG0;
    const u32 offs_ptr = ARG1;
    printf("cellGcmAddressToOffset(addr: 0x%08x, offs_ptr: 0x%08x)\n", addr, offs_ptr);

    u32 offs = 0;
    // Check if the address is in RSX memory
    if (Helpers::inRange<u32>(addr, gcm_config.local_addr, gcm_config.local_addr + gcm_config.local_size - 1)) {
        offs = addr - gcm_config.local_addr;
    }
    // Check if it's in IO region
    else if (Helpers::inRange<u32>(addr, gcm_config.io_addr, gcm_config.io_addr + gcm_config.io_size - 1)) {
        offs = addr - gcm_config.io_addr;
    }
    else
        Helpers::panic("cellGcmAddressToOffset: addr is not in rsx memory or io memory (0x%08x)\n", addr);

    ps3->mem.write<u32>(offs_ptr, offs);
    
    return Result::CELL_OK;
}

u64 CellGcmSys::cellGcmGetControlRegister() {
    printf("cellGcmGetControlRegister()\n");
    return ctrl_addr;
}

u64 CellGcmSys::cellGcmGetConfiguration() {
    const u32 config_ptr = ARG0;
    printf("cellGcmGetConfiguration(config_ptr: 0x%08x)\n", config_ptr);
    
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
    printf("cellGcmGetLabelAddress(idx: 0x%02x)\n", idx);

    return label_addr + 0x10 * idx;
}