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

    return Result::CELL_OK;
}