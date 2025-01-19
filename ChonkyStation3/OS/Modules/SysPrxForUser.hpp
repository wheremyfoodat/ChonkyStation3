#pragma once

#include <common.hpp>
#include <logger.hpp>
#include <BEField.hpp>

#include <CellTypes.hpp>


// Circular dependency
class PlayStation3;

using namespace CellTypes;

class SysPrxForUser {
public:
    SysPrxForUser(PlayStation3* ps3) : ps3(ps3) {}
    PlayStation3* ps3;

    u64 sysProcessExit();
    u64 sysProcessAtExitSpawn();
    u64 sysGetSystemTime();
    u64 sysProcess_At_ExitSpawn();
    u64 sysSpinlockInitialize();
    u64 sysSpinlockLock();
    u64 sysSpinlockUnlock();
    u64 sysMemset();
    u64 sysMemcpy();

private:
    MAKE_LOG_FUNCTION(log, sysPrxForUser);
};