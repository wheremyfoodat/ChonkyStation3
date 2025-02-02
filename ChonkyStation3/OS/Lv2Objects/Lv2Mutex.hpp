#pragma once

#include <common.hpp>

#include <Lv2Base.hpp>


class Lv2Mutex : public virtual Lv2Base {
public:
    Lv2Mutex(Lv2Object* obj) : Lv2Base(obj) {}

    u32 owner = -1; // -1 (0xffffffff) == free

    bool isFree();
    void lock(u32 owner);
    void unlock();
};