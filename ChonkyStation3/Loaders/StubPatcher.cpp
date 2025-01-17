#include "StubPatcher.hpp"


void StubPatcher::patch(u32 addr, bool lle, PlayStation3* ps3) {
    // Patch import stub
    // There are 2 different kinds of stubs, one uses bcctr, the other bcctrl, both are patched using syscalls.
    // We patch LLE stubs with a syscall as well. The syscall will then redirect the PPU to the appropiate function export.
    // While this isn't optimal (we could just patch the stub to directly jump to the function), it allows me to easily track LLE function calls.
    bool stubbed = false;
    for (int i = 0; i < 128; i += 4) {
        // Find BCCTR or BCCTRL instructions, and patch the module accordingly to whether it's HLE or LLE
        const auto instr = ps3->mem.read<u32>(addr + i);
        if (instr == 0x4e800420) {
            if (!lle) {
                ps3->mem.write<u32>(addr + i - 4, 0x39600010);   // li r11, 0x10
                ps3->mem.write<u32>(addr + i - 0, 0x44000000);   // sc
                stubbed = true;
            }
            else {
                ps3->mem.write<u32>(addr + i - 4, 0x39601000);   // li r11, 0x1000
                ps3->mem.write<u32>(addr + i - 0, 0x44000000);   // sc
                stubbed = true;
            }
            break;
        }
        else if (instr == 0x4e800421) {
            if (!lle) {
                ps3->mem.write<u32>(addr + i - 4, 0x39600011);   // li r11, 0x11
                ps3->mem.write<u32>(addr + i - 0, 0x44000000);   // sc
                stubbed = true;
            }
            else {
                ps3->mem.write<u32>(addr + i - 4, 0x39601000);   // li r11, 0x1001
                ps3->mem.write<u32>(addr + i - 0, 0x44000001);   // sc
                stubbed = true;
            }
            break;
        }
    }
    if (!stubbed) Helpers::panic("Couldn't patch stub\n");
}