#pragma once

#include <common.hpp>


class PlayStation3;

class Lv2Object {
public:
    Lv2Object(u64 handle) : handle(handle) {}
    u64 handle;

    template<typename T>
    void create() {
        data = (void*)(new T(this));
    }

    template<typename T>
    T* get() { return (T*)data; }
    
    void* data;
};