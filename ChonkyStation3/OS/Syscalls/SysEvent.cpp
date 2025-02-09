#include <Syscall.hpp>
#include "PlayStation3.hpp"


MAKE_LOG_FUNCTION(log, sysEvent);

u64 Syscall::sysEventQueueCreate() {
    const u32 queue_id_ptr = ARG0;
    const u32 attr_ptr = ARG1;
    const u64 ipc_key = ARG2;
    const s32 size = ARG3;
    log("sys_event_queue_create(queue_id_ptr: 0x%08x, attr_ptr: 0x%08x, ipc_key: 0x%016x, size: 0x%08x) STUBBED\n", queue_id_ptr, attr_ptr, ipc_key, size);

    ps3->mem.write<u32>(queue_id_ptr, ps3->handle_manager.request());
    return Result::CELL_OK;
}

u64 Syscall::sysEventQueueReceive() {
    unimpl("sys_event_queue_receive() UNIMLPEMENTED\n");
    return Result::CELL_OK;
}

u64 Syscall::sysEventPortCreate() {
    const u32 port_id_ptr = ARG0;
    const s32 port_type = ARG1;
    const u64 name = ARG2;
    log("sys_event_port_create(port_id_ptr: 0x%08x, port_type: %d, name: 0x%016x) STUBBED\n", port_id_ptr, port_type, name);

    ps3->mem.write<u32>(port_id_ptr, ps3->handle_manager.request());
    return Result::CELL_OK;
}

u64 Syscall::sysEventPortConnectLocal() {
    const u32 port_id = ARG0;
    const u32 queue_id = ARG1;
    log("sys_event_port_connect_local(port_id: %d, queue_id: %d) STUBBED\n", port_id, queue_id);

    return Result::CELL_OK;
}