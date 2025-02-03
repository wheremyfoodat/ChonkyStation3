#pragma once

#include <common.hpp>

#include <Lv2Base.hpp>


class Lv2Semaphore : public virtual Lv2Base {
public:
    Lv2Semaphore(Lv2Object* obj) : Lv2Base(obj) {}

    u32 val = 0;
    u32 max_val = 0;

    void post(u32 val);
    void wait();
};