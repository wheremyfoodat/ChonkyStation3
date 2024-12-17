#include "Thread.hpp"
#include "ThreadManager.hpp"
#include "PlayStation3.hpp"


Thread::Thread(u64 entry, u64 stack_size, u64 arg, u8* name, u32 id, u32 tls_vaddr, u32 tls_filesize, u32 tls_memsize, ThreadManager* mgr) : mgr(mgr) {
    const u32 real_entry = mgr->ps3->mem.read<u32>(entry);
    this->id = id;
    this->name = Helpers::readString(name);
    stack = mgr->allocateStack(stack_size + 8);
    this->stack_size = stack_size + 8;  // Add 8 because we simulate 1 empty argument and 1 of whatever env is

    const u64 sp = STACK_REGION_START + stack_size + 8;
    state.pc = real_entry;
    state.gprs[0] = state.pc;
    state.gprs[1] = sp;
    state.gprs[2] = mgr->ps3->mem.read<u32>(entry + 4);

    // Initialize TLS
    // TODO: I'm not supposed to do this always but I don't know when
    // maybe when loading PRXs?
    mgr->ps3->module_manager.sysThread.initializeTLS(id, tls_vaddr, tls_filesize, tls_memsize, state);

    // Arguments
    state.gprs[3] = 1;  // argc
    state.gprs[4] = sp - 4;  // argv
    // Fake 1 argument
    u32 empty = 0;
    std::memcpy(mgr->ps3->mem.getPtrPhys(stack - 4), &empty, 4);

    // Environment (environment variables...? That's what I gather from PSL1GHT)
    state.gprs[5] = sp - 8;  // env pointer
    state.gprs[6] = 1;  // env count
    std::memcpy(mgr->ps3->mem.getPtrPhys(stack - 8), &empty, 4);

    state.gprs[7] = id;
    state.gprs[8] = tls_vaddr;
    state.gprs[9] = tls_filesize;
    state.gprs[10] = tls_memsize;

    state.gprs[28] = state.gprs[4];
    state.gprs[29] = state.gprs[3];
    state.gprs[31] = state.gprs[5];
}