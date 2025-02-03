#include <Syscall.hpp>
#include "PlayStation3.hpp"


MAKE_LOG_FUNCTION(log, sysCond_sc);

u64 Syscall::sysCondCreate() {
    const u32 cond_id_ptr = ARG0;
    const u32 mutex_id = ARG1;
    const u32 cond_attr_ptr = ARG2;
    log("sysCondCreate(cond_id_ptr: 0x%08x, mutex_id: %d, cond_attr_ptr: 0x%08x)\n", cond_id_ptr, mutex_id, cond_attr_ptr);

    Lv2Cond* cond = ps3->lv2_obj.create<Lv2Cond>();
    cond->mutex = mutex_id;
    ps3->mem.write<u32>(cond_id_ptr, cond->handle());

    return Result::CELL_OK;
}