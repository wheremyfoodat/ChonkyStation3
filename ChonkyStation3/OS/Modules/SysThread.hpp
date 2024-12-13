#pragma once

#include <common.hpp>
#include <CellTypes.hpp>
#include <BEField.hpp>


// Circular dependency
class PlayStation3;

using namespace CellTypes;

class SysThread {
public:
    SysThread(PlayStation3* ps3) : ps3(ps3) {}
    PlayStation3* ps3;

    u64 sysThreadGetID();
};