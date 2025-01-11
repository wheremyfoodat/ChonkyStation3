#include <Syscall.hpp>
#include "PlayStation3.hpp"


MAKE_LOG_FUNCTION(log, sysTimer);

u64 Syscall::sysTimerUsleep() {
    const u64 us = ARG0;
    log("sysTimerUsleep(us: %d)\n", us);

    ps3->thread_manager.current_thread->sleep(us);

    return Result::CELL_OK;
}