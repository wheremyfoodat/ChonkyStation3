#pragma once

#include <common.hpp>

#include <Lv2Base.hpp>


class Lv2Cond : public virtual Lv2Base {
public:
    Lv2Cond(Lv2Object* obj) : Lv2Base(obj) {}

    u32 mutex;
};