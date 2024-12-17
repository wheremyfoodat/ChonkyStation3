#include "Syscall.hpp"
#include <PlayStation3.hpp>


Syscall::Syscall(PlayStation3* ps3) {
    this->ps3 = ps3;
}

void Syscall::doSyscall(bool decrement_pc_if_module_call) {
    const auto syscall_num = ps3->ppu->state.gprs[11];

    switch (syscall_num) {
    
    // Module call
    case 0x10:
    case 0x11: {
        bool should_return = ps3->ppu->state.gprs[11] == 0x10;
        // Normally the stub does this to jump to the function (mtctr + bctr), I don't think it matters but to be sure I added this here
        ps3->ppu->state.ctr = ps3->ppu->state.gprs[0];
        // import addr is stored in r12
        ps3->module_manager.call(ps3->module_manager.imports[ps3->ppu->state.gprs[12]]);
        // return
        if (should_return) ps3->ppu->state.pc = ps3->ppu->state.lr;
        if (decrement_pc_if_module_call && should_return) ps3->ppu->state.pc -= 4;
        break;
    }

    case 0x14a: ps3->ppu->state.gprs[3] = sysMMapperAllocateAddress();      break;
    case 0x14b: ps3->ppu->state.gprs[3] = Result::CELL_OK;  printf("sysMMapperFreeAddress() UNIMPLEMENTED\n");  break;
    case 0x151: ps3->ppu->state.gprs[3] = sysMMapperSearchAndMapMemory();   break;
    case 0x160: ps3->ppu->state.gprs[3] = sysMemoryGetUserMemorySize();     break;
    case 0x193: {   // puts
        printf("0x%08llx %lld @ 0x%016llx\n", ARG1, ARG2, ps3->ppu->state.pc);
        std::string str;
        u8* ptr = ps3->mem.getPtr(ARG1);
        for (int i = 0; i < ARG2; i++)
            str += *ptr++;
        std::printf("%s", str.c_str());
        std::puts(Helpers::readString(ps3->mem.getPtr(ARG1)).c_str());
        ps3->ppu->state.gprs[3] = Result::CELL_OK;
        break;
    }
    case 0x329: {
        printf("cellFsFStat() UNIMPLEMENTED\n");
        ps3->ppu->state.gprs[3] = Result::CELL_OK;
    }
    case 0x3dc: ps3->ppu->state.gprs[3] = Result::CELL_OK;  break;

    default:
        Helpers::panic("Unimplemented syscall number 0x%02x @ 0x%016llx\n", syscall_num, ps3->ppu->state.pc);
    }
}