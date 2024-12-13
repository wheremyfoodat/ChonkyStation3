#include "SysThread.hpp"
#include "PlayStation3.hpp"


u64 SysThread::sysThreadGetID() {
    u32 ptr = ARG0;
    printf("sysThreadGetID(ptr: 0x%08x)\n", ptr);

    ps3->mem.write<u64>(ptr, ps3->thread_manager.current_thread->id);
    return Result::CELL_OK;
}