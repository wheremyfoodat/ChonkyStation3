#include <Syscall.hpp>
#include "PlayStation3.hpp"


MAKE_LOG_FUNCTION(log, sys_spu);

u64 Syscall::_sys_spu_image_import() {
    const u32 image_ptr = ARG0;
    const u32 src = ARG1;
    const u32 size = ARG2;
    const u32 arg4 = ARG3;
    log("sys_spu_image_import(image_ptr: 0x%08x, src: 0x%08x, size: 0x%08x, arg4: 0x%08x) UNIMPLEMENTED\n");

    return Result::CELL_OK;
}