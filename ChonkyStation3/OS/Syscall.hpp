#pragma once

#include <common.hpp>
#include <logger.hpp>

#include <CellTypes.hpp>
#include <Lv2Objects/Lv2Mutex.hpp>
#include <Lv2Objects/Lv2Cond.hpp>
#include <Lv2Objects/Lv2Semaphore.hpp>


// Circular dependency
class PlayStation3;

using namespace CellTypes;

class Syscall {
public:
    Syscall(PlayStation3* ps3);
    PlayStation3* ps3;
    MAKE_LOG_FUNCTION(log_misc, misc_sc);
    MAKE_LOG_FUNCTION(unimpl, unimplemented);
    MAKE_LOG_FUNCTION(tty, tty);

    void doSyscall(bool decrement_pc_if_module_call = false);

    void todo(std::string name);
    // sysMMapper
    u64 sysMMapperAllocateAddress();
    u64 sysMMapperSearchAndMapMemory();

    // sysMemory
    u64 sysMemoryAllocate();
    u64 sysMemoryGetUserMemorySize();

    // sysTimer
    u64 sysTimerUsleep();
    u64 sysTimerSleep();

    // sysEvent
    u64 sysEventQueueCreate();
    u64 sysEventQueueReceive();
    u64 sysEventPortCreate();
    u64 sysEventPortConnectLocal();

    // sysSemaphore
    u64 sysSemaphoreCreate();
    u64 sysSemaphoreWait();
    u64 sysSemaphorePost();

    // sysMutex
    u64 sysMutexCreate();
    u64 sysMutexLock();
    u64 sysMutexUnlock();

    // sysCond
    u64 sysCondCreate();

    // Temporary until I move the FS syscalls to their own file
    enum CELL_FS_S : u32 {
        CELL_FS_S_IFDIR = 0040000,	// Directory
        CELL_FS_S_IFREG = 0100000,	// Regular
        CELL_FS_S_IFLNK = 0120000,	// Symbolic link
        CELL_FS_S_IFWHT = 0160000,	// Unknown

        CELL_FS_S_IRUSR = 0000400,	// R for owner
        CELL_FS_S_IWUSR = 0000200,	// W for owner
        CELL_FS_S_IXUSR = 0000100,	// X for owner

        CELL_FS_S_IRGRP = 0000040,	// R for group
        CELL_FS_S_IWGRP = 0000020,	// W for group
        CELL_FS_S_IXGRP = 0000010,	// X for group

        CELL_FS_S_IROTH = 0000004,	// R for other
        CELL_FS_S_IWOTH = 0000002,	// W for other
        CELL_FS_S_IXOTH = 0000001,	// X for other
    };
};