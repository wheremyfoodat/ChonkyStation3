#include "Thread.hpp"
#include "ThreadManager.hpp"
#include "PlayStation3.hpp"


Thread::Thread(u64 entry, u64 stack_size, u64 arg, u8* name, int id, u32 tls_vaddr, u32 tls_filesize, u32 tls_memsize, ThreadManager* mgr) : mgr(mgr) {
    const u32 real_entry = mgr->ps3->mem.read<u32>(entry);
    this->id = id;
    this->name = Helpers::readString(name);
    stack = mgr->allocateStack(stack_size);
    this->stack_size = stack_size;

    const u64 sp = STACK_REGION_START + stack_size;
    state.pc = real_entry;
    state.gprs[1] = sp;
    state.gprs[2] = mgr->ps3->mem.read<u32>(entry + 4);
    state.gprs[0] = state.pc;
    state.gprs[8] = entry;

    state.gprs[3] = 1;  // argc
    state.gprs[4] = 0;  // argv
    state.gprs[5] = 0;  // ??
    state.gprs[6] = 0;  // ??
    state.gprs[7] = id;
    state.gprs[8] = tls_vaddr;
    state.gprs[9] = tls_filesize;
    state.gprs[10] = tls_memsize;

    state.gprs[28] = state.gprs[4];
    state.gprs[29] = state.gprs[3];
    state.gprs[31] = state.gprs[5];
}