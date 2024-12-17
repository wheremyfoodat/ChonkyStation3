#pragma once

#include <common.hpp>
#include <CellTypes.hpp>
#include <BEField.hpp>


// Circular dependency
class PlayStation3;

using namespace CellTypes;

class SysLwMutex {
public:
    SysLwMutex(PlayStation3* ps3) : ps3(ps3) {}
    PlayStation3* ps3;

    struct LwMutex {
        BEField<u32> owner;
        BEField<u32> waiter;

        BEField<u32> attribute;
        BEField<u32> recursive_count;
        BEField<u32> sleep_queue;
    };

    struct LwMutexAttrib {
        BEField<u32> protocol;
        BEField<u32> recursive;
        u8 name[8];
    };

    u64 sysLwMutexCreate();
    u64 sysLwMutexLock();
    u64 sysLwMutexUnlock();
};