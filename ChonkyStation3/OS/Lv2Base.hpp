#pragma once

#include <common.hpp>


// Circular dependency
class Lv2Object;
class PlayStation3;

class Lv2Base {
public:
    Lv2Base(Lv2Object* obj) : obj(obj) {}
    Lv2Object* obj;

    u64 handle();
};