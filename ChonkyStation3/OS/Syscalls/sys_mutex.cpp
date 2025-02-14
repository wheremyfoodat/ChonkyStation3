#include <Syscall.hpp>
#include "PlayStation3.hpp"


MAKE_LOG_FUNCTION(log, sys_mutex);

u64 Syscall::sys_mutex_create() {
    const u32 mutex_id_ptr = ARG0;
    const u32 attr_ptr = ARG1;
    log("sys_mutex_create(mutex_id_ptr: 0x%08x, attr_ptr: 0x%08x)\n", mutex_id_ptr, attr_ptr);

    Lv2Mutex* mutex = ps3->lv2_obj.create<Lv2Mutex>();
    ps3->mem.write<u32>(mutex_id_ptr, mutex->handle());

    return Result::CELL_OK;
}

u64 Syscall::sys_mutex_lock() {
    const u32 mutex_id = ARG0;
    const u64 timeout = ARG1;
    log("sys_mutex_lock(mutex_idx: %d, timeout: %d)\n", mutex_id, timeout);

    Lv2Mutex* mutex = ps3->lv2_obj.get<Lv2Mutex>(mutex_id);
    if (mutex->isFree()) {
        mutex->lock(ps3->thread_manager.getCurrentThread()->id);
    }
    else {
        // TODO
        Helpers::panic("Tried to lock locked mutex\n");
    }

    return Result::CELL_OK;
}

u64 Syscall::sys_mutex_unlock() {
    const u32 mutex_id = ARG0;
    log("sys_mutex_unlock(mutex_id: %d)\n", mutex_id);

    Lv2Mutex* mutex = ps3->lv2_obj.get<Lv2Mutex>(mutex_id);
    if (mutex->owner == ps3->thread_manager.getCurrentThread()->id) {
        mutex->unlock();
    }
    else {
        // TODO
        Helpers::panic("Tried to unlock a mutex, but the mutex was already unlocked or the current thread is not the mutex's owner (owner: %d)\n", mutex->owner);
    }

    return Result::CELL_OK;
}