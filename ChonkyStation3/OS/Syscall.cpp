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
        //ps3->ppu->state.pc -= 4;
        return;
    }
    }

    switch (syscall_num) {
    
    // cellGcmCallback
    case 0x06: ps3->module_manager.cellGcmSys.cellGcmCallback(); break;

    case 1: {
        log_misc("sys_process_getpid()\n");
        ps3->ppu->state.gprs[3] = 1;
        break;
    }
    case 25: {
        const u32 pid = ARG0;
        const u32 ver_ptr = ARG1;
        // TODO: Get this from the elf's PROC_PARAM
        unimpl("sys_process_get_sdk_version() STUBBED\n");
        ps3->mem.write<u32>(ver_ptr, 0x00360001);
        ps3->ppu->state.gprs[3] = Result::CELL_OK;
        break;
    }
    case 30:    unimpl("_sys_process_get_paramsfo() UNIMPLEMENTED\n"); ps3->ppu->state.gprs[3] = Result::CELL_OK;  break;
    case 48: {
        log_misc("sys_ppu_thread_get_priority() STUBBED\n");
        ps3->mem.write<u32>(ARG1, 1);
        ps3->ppu->state.gprs[3] = Result::CELL_OK;
        break;
    }
    case 82:    unimpl("sys_event_flag_create() UNIMPLEMENTED\n");      ps3->ppu->state.gprs[3] = Result::CELL_OK;  break;
    case 90:    ps3->ppu->state.gprs[3] = sysSemaphoreCreate();         break;
    case 92:    ps3->ppu->state.gprs[3] = sysSemaphoreWait();           break;
    case 94:    ps3->ppu->state.gprs[3] = sysSemaphorePost();           break;
    case 95:    unimpl("_sys_lwmutex_create() UNIMPLEMENTED\n");        ps3->ppu->state.gprs[3] = Result::CELL_OK;  break;
    case 96:    unimpl("_sys_lwmutex_destroy() UNIMPLEMENTED\n");       ps3->ppu->state.gprs[3] = Result::CELL_OK;  break;
    case 97:    unimpl("_sys_lwmutex_lock() UNIMPLEMENTED\n");          ps3->ppu->state.gprs[3] = Result::CELL_OK;  break;
    case 98:    unimpl("_sys_lwmutex_unlock() UNIMPLEMENTED\n");        ps3->ppu->state.gprs[3] = Result::CELL_OK;  break;
    case 100:   ps3->ppu->state.gprs[3] = sysMutexCreate();             break;
    case 102:   ps3->ppu->state.gprs[3] = sysMutexLock();               break;
    case 104:   ps3->ppu->state.gprs[3] = sysMutexUnlock();             break;
    case 105:   ps3->ppu->state.gprs[3] = sysCondCreate();              break;
    case 120:   unimpl("sys_rwlock_create() UNIMPLEMENTED\n");          ps3->ppu->state.gprs[3] = Result::CELL_OK;  break;
    case 128:   ps3->ppu->state.gprs[3] = sysEventQueueCreate();        break;
    case 130:   ps3->ppu->state.gprs[3] = sysEventQueueReceive();       break;
    case 134:   ps3->ppu->state.gprs[3] = sysEventPortCreate();         break;
    case 136:   ps3->ppu->state.gprs[3] = sysEventPortConnectLocal();   break;
    case 141:   ps3->ppu->state.gprs[3] = sysTimerUsleep();             break;
    case 142:   ps3->ppu->state.gprs[3] = sysTimerSleep();              break;
    case 144:   unimpl("sys_time_get_timezone() UNIMPLEMENTED\n");      ps3->ppu->state.gprs[3] = Result::CELL_OK;  break;
    case 145: {
        log_misc("sysTimeGetCurrentTime() STUBBED\n");
        ps3->mem.write<u64>(ARG0, 1000);
        ps3->mem.write<u64>(ARG1, 1000);
        ps3->ppu->state.gprs[3] = Result::CELL_OK;
        break;
    }
    case 147: {
        log_misc("sys_time_get_timebase_frequency() UNIMPLEMENTED\n");
        ps3->ppu->state.gprs[3] = 1000;
        break;
    }
    case 160:   unimpl("sys_raw_spu_create() UNIMPLEMENTED\n");             ps3->ppu->state.gprs[3] = Result::CELL_OK;  break;
    case 169:   unimpl("sys_spu_initialize() UNIMPLEMENTED\n");             ps3->ppu->state.gprs[3] = Result::CELL_OK;  break;
    case 170:   unimpl("sys_spu_thread_group_create() UNIMPLEMENTED\n");    ps3->ppu->state.gprs[3] = Result::CELL_OK;  break;
    case 172:   unimpl("sys_spu_thread_initialize() UNIMPLEMENTED\n");      ps3->ppu->state.gprs[3] = Result::CELL_OK;  break;
    case 173:   unimpl("sys_spu_thread_group_start() UNIMPLEMENTED\n");     ps3->ppu->state.gprs[3] = Result::CELL_OK;  break;
    case 174:   unimpl("sys_spu_thread_group_suspend() UNIMPLEMENTED\n");   ps3->ppu->state.gprs[3] = Result::CELL_OK;  break;
    case 182:   unimpl("sys_spu_thread_read_ls() UNIMPLEMENTED\n");         ps3->ppu->state.gprs[3] = Result::CELL_OK;  break;
    case 190:   unimpl("sys_spu_thread_write_spu_mb() UNIMPLEMENTED\n");    ps3->ppu->state.gprs[3] = Result::CELL_OK;  break;
    case 254:   unimpl("sys_spu_thread_group_log() UNIMPLEMENTED\n");       ps3->ppu->state.gprs[3] = Result::CELL_OK;  break;
    case 330:   ps3->ppu->state.gprs[3] = sysMMapperAllocateAddress();      break;
    case 324:   unimpl("sys_memory_container_create() UNIMPLEMENTED\n");    ps3->ppu->state.gprs[3] = Result::CELL_OK;  break;
    case 331:   unimpl("sys_mmapper_free_address() UNIMPLEMENTED\n");       ps3->ppu->state.gprs[3] = Result::CELL_OK;  break;
    case 337:   ps3->ppu->state.gprs[3] = sysMMapperSearchAndMapMemory();   break;
    case 341:   unimpl("sys_memory_container_create() UNIMPLEMENTED\n");    ps3->ppu->state.gprs[3] = Result::CELL_OK;  break;
    case 348:   ps3->ppu->state.gprs[3] = sysMemoryAllocate();              break;
    case 352:   ps3->ppu->state.gprs[3] = sysMemoryGetUserMemorySize();     break;
    case 403: {   // puts
        std::string str;
        u8* ptr = ps3->mem.getPtr(ARG1);
        for (int i = 0; i < ARG2; i++)
            str += *ptr++;
        tty("%s", str.c_str());
        ps3->ppu->state.gprs[3] = Result::CELL_OK;
        break;
    }
    case 462:   ps3->ppu->state.gprs[3] = Result::CELL_OK;  break;  // Debug syscall unavailable on retail consoles (liblv2 tries to use this)
    case 480: {
        const u32 name_ptr = ARG0;
        const u64 flags = ARG1;
        const u32 opt_ptr = ARG2;
        std::string name = Helpers::readString(ps3->mem.getPtr(name_ptr));
        unimpl("_sys_prx_load_module(name_ptr: 0x%08x, flags: 0x%016llx, opt_ptr: 0x%08x) [name: %s] UNIMPLEMENTED\n", name_ptr, flags, opt_ptr, name.c_str());

        ps3->ppu->state.gprs[3] = ps3->handle_manager.request();
        break;
    }
    case 481:   ps3->ppu->state.gprs[3] = Result::CELL_OK;    unimpl("_sys_prx_start_module() UNIMPLEMENTED\n");  break;
    case 484: {
        const u32 name_ptr = ARG0;
        const u32 opt_ptr = ARG1;
        std::string name = Helpers::readString(ps3->mem.getPtr(name_ptr));
        unimpl("_sys_prx_register_module(name_ptr: 0x%08x, opt_ptr: 0x%08x) [name: %s] UNIMPLEMENTED\n", name_ptr, opt_ptr, name.c_str());

        ps3->ppu->state.gprs[3] = Result::CELL_OK;
        break;
    }
    case 486:   ps3->ppu->state.gprs[3] = Result::CELL_OK;    unimpl("_sys_prx_register_library() UNIMPLEMENTED\n");  break;
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