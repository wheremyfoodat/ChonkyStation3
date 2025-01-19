#include "SysLwMutex.hpp"
#include "PlayStation3.hpp"


u64 SysLwMutex::sysLwMutexLock() {
    const u64 ptr = ARG0;
    const u64 timeout = ARG1;
    log("sysLwMutexLock(ptr: 0x%08llx, timeout: %lld)\n", ptr, timeout);

    LwMutex* mtx = (LwMutex*)ps3->mem.getPtr(ptr);
    if (mtx->owner != -1) {
        // TODO: recursion
        //Helpers::panic("Tried to lock already locked mutex\n");
        log("Tried to lock already locked mutex\n");
    }

    mtx->owner = ps3->thread_manager.getCurrentThread()->id;   // owner

    return Result::CELL_OK;
}

u64 SysLwMutex::sysLwMutexUnlock() {
    const u64 ptr = ARG0;
    //log("sysLwMutexUnlock(ptr: 0x%08llx)\n", ptr);

    LwMutex* mtx = (LwMutex*)ps3->mem.getPtr(ptr);
    // TODO: should I actually check for this?
    if (mtx->owner == -1) {
        //Helpers::panic("Tried to unlock already unlocked mutex\n");
        //log("Tried to unlock already unlocked mutex\n");
    }

    mtx->owner = -1;  // free

    return Result::CELL_OK;
}

u64 SysLwMutex::sysLwMutexCreate() {
    const u64 ptr = ARG0;
    const u64 attrib_ptr = ARG1;
    LwMutexAttrib* attrib = (LwMutexAttrib*)ps3->mem.getPtr(attrib_ptr);
    auto name = Helpers::readString(attrib->name);
    log("sysLwMutexCreate(ptr: 0x%08llx, attrib_ptr: 0x%08llx (protocol: 0x%08x, recursive: 0x%08x, name: \"%s\")\n", ptr, attrib_ptr, (u32)attrib->protocol, (u32)attrib->recursive, name.c_str());

    LwMutex* mtx = (LwMutex*)ps3->mem.getPtr(ptr);
    mtx->owner = -1;    // free
    mtx->waiter = 0;
    mtx->attribute = attrib->protocol | attrib->recursive;
    mtx->recursive_count = 0;
    mtx->sleep_queue = 0;
    
    return Result::CELL_OK;
}