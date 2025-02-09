#include <Syscall.hpp>
#include "PlayStation3.hpp"


MAKE_LOG_FUNCTION(log, sysTimer);

u64 Syscall::sysTimerUsleep() {
    const u64 us = ARG0;
    log("sysTimerUsleep(us: %d) @ 0x%08x\n", us, (u32)ps3->ppu->state.pc);

    ps3->thread_manager.getCurrentThread()->sleep(us);

    return Result::CELL_OK;
}

u64 Syscall::sysTimerSleep() {
    const u64 s = ARG0;
    log("sysTimerSleep(s: %d)\n", s);

    ps3->thread_manager.getCurrentThread()->sleep(s * 1000000);

    return Result::CELL_OK;
}