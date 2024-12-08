#include "PPU.hpp"


void PPU::step() {
    Helpers::panic("Backend did not define step function\n");
}

bool PPU::branchCondition(u8 bo, u8 bi) {
    // BO bit 4: don't test CR if set
    // BO bit 2: don't test CTR (and don't decrement) if set
    // BO bit 3: if bit 4 is unset (meaning the condition will test for CR), check if CR bit bi is equal to this bit
    // BO bit 1: if bit 2 is unset (meaning the condition will test for CTR), and this bit is set, check if CTR == 0. If this bit is unset, condition is CTR != 0
    // So if bits 4 and 2 are set nothing is tested (condition is always true)
    // The manual does a poor job at explaining what the individual bits do in my opinion

    const auto bit1 = (bo >> 1) & 1;
    const auto bit2 = (bo >> 2) & 1;
    const auto bit3 = (bo >> 3) & 1;
    const auto bit4 = (bo >> 4) & 1;

    if (bit2 && bit4) return true;
    const bool crCheck = bit4 || (((state.cr.raw >> (31 - bi)) & 1) == bit3);
    const bool ctrCheck = bit2 || (bit1 ? (state.ctr == 0) : (state.ctr != 0));
    return crCheck && ctrCheck;
}