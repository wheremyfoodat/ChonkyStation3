#include "Syscall.hpp"
#include <PlayStation3.hpp>


Syscall::Syscall(PlayStation3* ps3) {
    this->ps3 = ps3;
}

void Syscall::doSyscall(bool decrement_pc_if_module_call) {
    const auto syscall_num = ps3->ppu->state.gprs[11];

    const u16 stub = ps3->mem.read<u32>(ps3->ppu->state.pc) & 0xffff;
    switch (stub) {
    // Module call
    case 0x10: {
        // Normally the stub does this to jump to the function (mtctr + bctr), I don't think it matters but to be sure I added this here
        ps3->ppu->state.ctr = ps3->ppu->state.gprs[0];
        // import addr is stored in r12
        ps3->module_manager.call(ps3->module_manager.imports[ps3->ppu->state.gprs[12]]);
        return;
    }
    case 0x2010: {
        // Normally the stub does this to jump to the function (mtctr + bctr), I don't think it matters but to be sure I added this here
        ps3->ppu->state.ctr = ps3->ppu->state.gprs[0];

        const u32 stub_addr = ps3->mem.read<u32>(ps3->ppu->state.gprs[2]);
        ps3->module_manager.call(ps3->module_manager.imports[stub_addr]);
        return;
    }
    case 0x1010: {
        ps3->module_manager.lle(ps3->module_manager.imports[ps3->ppu->state.gprs[12]]);
        ps3->ppu->state.pc -= 4;
        return;
    }
    }

    switch (syscall_num) {
    
    // cellGcmCallback
    case 0x06: ps3->module_manager.cellGcmSys.cellGcmCallback(); break;

    case 1: {
        log("sysProcessGetPID()\n");
        ps3->ppu->state.gprs[3] = 1;
        break;
    }
    case 25: {
        const u32 pid = ARG0;
        const u32 ver_ptr = ARG1;
        // TODO: Get this from the elf's PROC_PARAM
        unimpl("sysProcessGetSDKVersion() STUBBED\n");
        ps3->mem.write<u32>(ver_ptr, 0x00360001);
        ps3->ppu->state.gprs[3] = Result::CELL_OK;
        break;
    }
    case 48: {
        log("sysPpuThreadGetPriority() STUBBED\n");
        ps3->mem.write<u32>(ARG1, 1);
        ps3->ppu->state.gprs[3] = Result::CELL_OK;
        break;
    }
    case 82:   unimpl("sysEventFlagCreate() UNIMPLEMENTED\n");  ps3->ppu->state.gprs[3] = Result::CELL_OK;  break;
    case 90:   unimpl("sysSemaphoreCreate() UNIMPLEMENTED\n");  ps3->ppu->state.gprs[3] = Result::CELL_OK;  break;
    case 92:   unimpl("sysSemaphoreWait() UNIMPLEMENTED\n");  ps3->ppu->state.gprs[3] = Result::CELL_OK;  break;
    case 94:   unimpl("sysSemaphorePost() UNIMPLEMENTED\n");  ps3->ppu->state.gprs[3] = Result::CELL_OK;  break;
    case 120:  unimpl("sysRwlockCreate() UNIMPLEMENTED\n");  ps3->ppu->state.gprs[3] = Result::CELL_OK;  break;
    case 128:  ps3->ppu->state.gprs[3] = sysEventQueueCreate();         break;
    case 130:  ps3->ppu->state.gprs[3] = sysEventQueueReceive();         break;
    case 134:  ps3->ppu->state.gprs[3] = sysEventPortCreate();          break;
    case 136:  ps3->ppu->state.gprs[3] = sysEventPortConnectLocal();    break;
    case 141:  ps3->ppu->state.gprs[3] = sysTimerUsleep();  break;
    case 144:  unimpl("sysTimeGetTimezone() UNIMPLEMENTED\n");  ps3->ppu->state.gprs[3] = Result::CELL_OK;  break;
    case 145: {
        log("sysTimeGetCurrentTime() STUBBED\n");
        ps3->mem.write<u64>(ARG0, 1000);
        ps3->mem.write<u64>(ARG1, 1000);
        ps3->ppu->state.gprs[3] = Result::CELL_OK;
        break;
    }
    case 169:  unimpl("sysSpuInitialize() UNIMPLEMENTED\n"); ps3->ppu->state.gprs[3] = Result::CELL_OK;  break;
    case 147: {
        log("sysTimeGetTimebaseFrequency() UNIMPLEMENTED\n");
        ps3->ppu->state.gprs[3] = 1000;
        break;
    }
    case 182: ps3->ppu->state.gprs[3] = Result::CELL_OK;    unimpl("sysSpuThreadWriteSnr() UNIMPLEMENTED\n");   break;
    case 190: ps3->ppu->state.gprs[3] = Result::CELL_OK;    unimpl("sysSpuThreadWriteSpuMb() UNIMPLEMENTED\n"); break;
    case 330: ps3->ppu->state.gprs[3] = sysMMapperAllocateAddress();      break;
    case 331: ps3->ppu->state.gprs[3] = Result::CELL_OK;  log("sysMMapperFreeAddress() UNIMPLEMENTED\n");  break;
    case 337: ps3->ppu->state.gprs[3] = sysMMapperSearchAndMapMemory();   break;
    case 348: ps3->ppu->state.gprs[3] = sysMemoryAllocate();     break;
    case 352: ps3->ppu->state.gprs[3] = sysMemoryGetUserMemorySize();     break;
    case 403: {   // puts
        std::string str;
        u8* ptr = ps3->mem.getPtr(ARG1);
        for (int i = 0; i < ARG2; i++)
            str += *ptr++;
        tty("%s", str.c_str());
        ps3->ppu->state.gprs[3] = Result::CELL_OK;
        break;
    }
    case 801:   ps3->ppu->state.gprs[3] = ps3->module_manager.cellFs.cellFsOpen();  break;
    case 802:   ps3->ppu->state.gprs[3] = ps3->module_manager.cellFs.cellFsRead();  break;
    case 804:   ps3->ppu->state.gprs[3] = ps3->module_manager.cellFs.cellFsClose(); break;
    case 808:   ps3->ppu->state.gprs[3] = ps3->module_manager.cellFs.cellFsStat();  break;
    case 809: {
        const u32 fd = ARG0;
        const u32 stat_ptr = ARG1;

        if (fd == 1) {  // stdout?
            CellFsStat* stat = (CellFsStat*)ps3->mem.getPtr(stat_ptr);
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
            ps3->ppu->state.gprs[3] = Result::CELL_BADF;
        }
        else {
            ps3->ppu->state.gprs[3] = ps3->module_manager.cellFs.cellFsFstat();
        }
        break;
    }
    case 818:   ps3->ppu->state.gprs[3] = ps3->module_manager.cellFs.cellFsLseek();  break;
    case 988:   ps3->ppu->state.gprs[3] = Result::CELL_OK;  break;

    default:
        Helpers::panic("Unimplemented syscall number 0x%02x (%d) @ 0x%016llx\n", syscall_num, syscall_num, ps3->ppu->state.pc);
    }
}