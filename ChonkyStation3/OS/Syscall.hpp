#pragma once

#include <common.hpp>


// Circular dependency
class PlayStation3;

class Syscall {
public:
    Syscall(PlayStation3* ps3);
    PlayStation3* ps3;

    void doSyscall();
};