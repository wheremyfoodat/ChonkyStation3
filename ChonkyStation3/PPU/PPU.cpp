#include "PPU.hpp"


void PPU::step() {
    Helpers::panic("Backend did not define step function\n");
}

void PPU::runFunc(u32 addr, u32 toc) {
    const PPUTypes::State old_state = state;
    const u64 ret_val = 0xFFFF1000FFFF1000;
    state.lr = ret_val;  // Random value - we check that pc == this to know that the function returned
    state.pc = addr;
    if (toc) state.gprs[2] = toc;

    while (state.pc != ret_val)
        step();

    state = old_state;
}

void PPU::printState() {
    printf("pc:  0x%016llx\n", state.pc);
    printf("ctr: 0x%016llx\n", state.ctr);
    printf("cr:  0x%08x\n", state.cr.raw);
    for (int i = 0; i < 32; i++)
        printf("r%02d:  0x%016llx\n", i, state.gprs[i]);
    for (int i = 0; i < 32; i++)
        printf("f%02d:  %f\n", i, state.fprs[i]);
    for (int i = 0; i < 32; i++)
        printf("v%02d:  { 0x%08x, 0x%08x, 0x%08x, 0x%08x } (%f, %f, %f, %f)\n", i, state.vrs[i].w[3], state.vrs[i].w[2], state.vrs[i].w[1], state.vrs[i].w[0], *(float*)&state.vrs[i].w[3], *(float*)&state.vrs[i].w[2], *(float*)&state.vrs[i].w[1], *(float*)&state.vrs[i].w[0]);
}

bool PPU::branchCondition(u8 bo, u8 bi) {
    // BO bit 4: don't test CR if set
    // BO bit 2: don't test CTR (and don't decrement) if set
    // BO bit 3: if bit 4 is unset (meaning the condition will test for CR), check if CR bit bi is equal to this bit
    // BO bit 1: if bit 2 is unset (meaning the condition will test for CTR), and this bit is set, check if CTR == 0. If this bit is unset, condition is CTR != 0
    // So if bits 4 and 2 are set nothing is tested (condition is always true)
    // The manual does a poor job at explaining what the individual bits do in my opinion
    // Or I might just be dumb

    const auto bit1 = (bo >> 1) & 1;
    const auto bit2 = (bo >> 2) & 1;
    const auto bit3 = (bo >> 3) & 1;
    const auto bit4 = (bo >> 4) & 1;

    if (bit2 && bit4) return true;
    if (!bit2) state.ctr--;
    const bool crCheck = bit4 || (((state.cr.raw >> (31 - bi)) & 1) == bit3);
    const bool ctrCheck = bit2 || (bit1 ? (state.ctr == 0) : (state.ctr != 0));

    return crCheck && ctrCheck;
}

u32 PPU::rotationMask(u32 mb, u32 me) {
    const u32 begin = 0xFFFFFFFF >> mb;
    const u32 end = 0x7FFFFFFF >> me;
    const u32 mask = begin ^ end;

    if (me < mb)
        return ~mask;

    return mask;
}