#include "Syscall.hpp"
#include <PlayStation3.hpp>


Syscall::Syscall(PlayStation3* ps3) {
    this->ps3 = ps3;
}

void Syscall::doSyscall(bool decrement_pc_if_module_call) {
    const auto syscall_num = ps3->ppu->state.gprs[11];

    switch (syscall_num) {
    
    // Module call
    case 0x10: {
        // import addr is stored in r12
        ps3->module_manager.call(ps3->module_manager.imports[ps3->ppu->state.gprs[12]]);
        // return
        ps3->ppu->state.pc = ps3->ppu->state.lr;
        if (decrement_pc_if_module_call) ps3->ppu->state.pc -= 4;
        break;
    }

    case 0x160: ps3->ppu->state.gprs[3] = (u64)sysMemoryGetUserMemorySize(); break;

    default:
        Helpers::panic("Unimplemented syscall number 0x%02x\n", syscall_num);
    }

}