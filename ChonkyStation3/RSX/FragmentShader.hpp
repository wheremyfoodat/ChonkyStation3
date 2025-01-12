#pragma once

#include <common.hpp>


class FragmentShader {
public:
    FragmentShader() {}
    FragmentShader(u32 addr, u32 ctrl) : addr(addr), ctrl(ctrl) {}
    u32 addr;
    u32 ctrl;
};