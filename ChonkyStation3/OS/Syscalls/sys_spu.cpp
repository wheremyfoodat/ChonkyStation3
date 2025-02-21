#include <Syscall.hpp>
#include "PlayStation3.hpp"


MAKE_LOG_FUNCTION(log_sys_spu, sys_spu);

u64 Syscall::sys_raw_spu_create() {
    const u32 id_ptr = ARG0;
    const u32 attr_ptr = ARG1;
    log_sys_spu("sys_raw_spu_create(id_ptr: 0x%08x, attr_ptr: 0x%08x) STUBBED\n", id_ptr, attr_ptr);

    ps3->mem.write<u32>(id_ptr, 0);
    ps3->mem.spu.alloc(SPU_MEM_SIZE);
    
    return Result::CELL_OK;
}

u64 Syscall::_sys_spu_image_import() {
    const u32 image_ptr = ARG0;
    const u32 src = ARG1;
    const u32 size = ARG2;
    const u32 arg4 = ARG3;
    log_sys_spu("sys_spu_image_import(image_ptr: 0x%08x, src: 0x%08x, size: 0x%08x, arg4: 0x%08x) UNIMPLEMENTED\n");

    return Result::CELL_OK;
}

u64 Syscall::sys_spu_thread_group_join() {
    log_sys_spu("sys_spu_thread_group_join() UNIMPLEMENTED\n");

    ps3->thread_manager.getCurrentThread()->status = Thread::THREAD_STATUS::Sleeping;
    ps3->thread_manager.getCurrentThread()->reschedule();

    return Result::CELL_OK;
}

u64 Syscall::sys_spu_thread_group_connect_event_all_threads() {
    const u32 id = ARG0;
    const u32 eq = ARG1;
    const u64 req = ARG2;
    const u32 spup_ptr = ARG3;  // spup is u8
    log_sys_spu("sys_spu_thread_group_connect_event_all_threads(id: %d, eq: %d, req: %d, spup_ptr: 0x%08x) UNIMPLEMENTED\n", id, eq, req, spup_ptr);

    ps3->mem.write<u8>(spup_ptr, 0);
    return Result::CELL_OK;
}