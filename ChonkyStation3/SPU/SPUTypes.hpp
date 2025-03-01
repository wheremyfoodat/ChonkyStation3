#pragma once

#include <BitField.hpp>


namespace SPUTypes {

union GPR {
    u8 b[16];
    u16 h[8];
    u32 w[4];
    u64 dw[2];
    float f[4];
    double d[2];
};

struct State {
    GPR gprs[128] = { 0 };
    u32 pc = 0;
};

}   // End namespace SPUTypes