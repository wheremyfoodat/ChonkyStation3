#include <Syscall.hpp>
#include "PlayStation3.hpp"


MAKE_LOG_FUNCTION(log, sysEvent);

u64 Syscall::sysEventQueueCreate() {
    const u32 queue_id_ptr = ARG0;
    const u32 attr_ptr = ARG1;
    const u64 ipc_key = ARG2;
    const s32 size = ARG3;
    log("sysEventQueueCreate(queue_id_ptr: 0x%08x, attr_ptr: 0x%08x, ipc_key: 0x%016x, size: 0x%08x) STUBBED\n", queue_id_ptr, attr_ptr, ipc_key, size);

    ps3->mem.write<u32>(queue_id_ptr, ps3->handle_manager.request());
    return Result::CELL_OK;
}