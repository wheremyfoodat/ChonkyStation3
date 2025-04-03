#include <Syscall.hpp>
#include "PlayStation3.hpp"


MAKE_LOG_FUNCTION(log_sys_event_flag, sys_event_flag);

u64 Syscall::sys_event_flag_wait() {
    log_sys_event_flag("sys_event_flag_wait() UNIMPLEMENTED\n");
    //ps3->thread_manager.getCurrentThread()->wait();
    return Result::CELL_OK;
}