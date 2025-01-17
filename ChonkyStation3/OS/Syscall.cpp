#include "Syscall.hpp"
#include <PlayStation3.hpp>


Syscall::Syscall(PlayStation3* ps3) {
    this->ps3 = ps3;
}

void Syscall::doSyscall(bool decrement_pc_if_module_call) {
    const auto syscall_num = ps3->ppu->state.gprs[11];

    switch (syscall_num) {
    
    // cellGcmCallback
    case 0x06: ps3->module_manager.cellGcmSys.cellGcmCallback(); break;
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
    case 0x1000: {
        ps3->module_manager.lle(ps3->module_manager.imports[ps3->ppu->state.gprs[12]]);
        ps3->ppu->state.pc -= 4;
        break;
    }

    case 0x78:  unimpl("sysRwlockCreate()\n");  ps3->ppu->state.gprs[3] = Result::CELL_OK;  break;
    case 0x8d:
        ps3->ppu->state.gprs[3] = sysTimerUsleep(); break;
    case 0x91: {
        log("sysTimeGetCurrentTime() STUBBED\n");
        ps3->mem.write<u64>(ARG0, 1000);
        ps3->mem.write<u64>(ARG1, 1000);
        ps3->ppu->state.gprs[3] = Result::CELL_OK;
        break;
    }
    case 0x93: {
        log("sysTimeGetTimebaseFrequency() UNIMPLEMENTED\n");
        ps3->ppu->state.gprs[3] = 1000;
        break;
    }
    case 0x14a: ps3->ppu->state.gprs[3] = sysMMapperAllocateAddress();      break;
    case 0x14b: ps3->ppu->state.gprs[3] = Result::CELL_OK;  log("sysMMapperFreeAddress() UNIMPLEMENTED\n");  break;
    case 0x151: ps3->ppu->state.gprs[3] = sysMMapperSearchAndMapMemory();   break;
    case 0x15c: ps3->ppu->state.gprs[3] = sysMemoryAllocate();     break;
    case 0x160: ps3->ppu->state.gprs[3] = sysMemoryGetUserMemorySize();     break;
    case 0x193: {   // puts
        std::string str;
        u8* ptr = ps3->mem.getPtr(ARG1);
        for (int i = 0; i < ARG2; i++)
            str += *ptr++;
        tty("%s", str.c_str());
        ps3->ppu->state.gprs[3] = Result::CELL_OK;
        break;
    }
    case 0x329: {
        const u32 fd = ARG0;
        const u32 stat_ptr = ARG1;
        log("cellFsFStat(fd: 0x%08x, stat_ptr: 0x%08x) STUBBED\n", fd, stat_ptr);

        CellFsStat* stat = (CellFsStat*)ps3->mem.getPtr(stat_ptr);

        if (fd == 1) {  // stdout?
            stat->mode = CELL_FS_S_IRUSR | CELL_FS_S_IWUSR | CELL_FS_S_IXUSR |
                         CELL_FS_S_IRGRP | CELL_FS_S_IWGRP | CELL_FS_S_IXGRP |
                         CELL_FS_S_IROTH | CELL_FS_S_IWOTH | CELL_FS_S_IXOTH |
                         CELL_FS_S_IFREG;
            stat->uid = 0;
            stat->gid = 0;
            stat->atime = 0;
            stat->mtime = 0;
            stat->ctime = 0;
            stat->size = 0;
            stat->blksize = 4096;
        }
        ps3->ppu->state.gprs[3] = Result::CELL_BADF;
        break;
    }
    case 0x3dc: ps3->ppu->state.gprs[3] = Result::CELL_OK;  break;

    default:
        Helpers::panic("Unimplemented syscall number 0x%02x (%d) @ 0x%016llx\n", syscall_num, syscall_num, ps3->ppu->state.pc);
    }
}