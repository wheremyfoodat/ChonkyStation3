#pragma once

#include <common.hpp>
#include <CellTypes.hpp>
#include <BEField.hpp>


// Circular dependency
class PlayStation3;

using namespace CellTypes;

class SysPrxForUser {
public:
    SysPrxForUser(PlayStation3* ps3) : ps3(ps3) {}
    PlayStation3* ps3;

    struct SysLwMutexAttrib {
        BEField<u32> protocol;
        BEField<u32> recursive;
        u8 name[8];
    };

    Result sysLwMutexCreate();
    Result sysThreadInitializeTLS();
};