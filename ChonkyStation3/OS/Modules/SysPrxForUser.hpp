#pragma once

#include <common.hpp>
#include <CellTypes.hpp>


// Circular dependency
class PlayStation3;

using namespace CellTypes;

class SysPrxForUser {
public:
    SysPrxForUser(PlayStation3* ps3) : ps3(ps3) {}
    PlayStation3* ps3;

    Result sysLwMutexCreate();
    Result sysThreadInitializeTLS();
};