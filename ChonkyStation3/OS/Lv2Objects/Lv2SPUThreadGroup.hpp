#pragma once

#include <common.hpp>

#include <queue>

#include <Lv2Base.hpp>
#include <OS/Syscalls/sys_spu.hpp>


using namespace sys_spu;

// Circular dependency
class PlayStation3;

class Lv2SPUThreadGroup : public virtual Lv2Base {
public:
    Lv2SPUThreadGroup(Lv2Object* obj) : Lv2Base(obj) {}

    sys_spu_thread_group_attribute* attr;
    std::vector<u32> threads;
    bool started = false;

    bool start(PlayStation3* ps3);
    std::string getName(PlayStation3* ps3);
};