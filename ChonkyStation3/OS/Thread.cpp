#include "Thread.hpp"
#include "ThreadManager.hpp"


Thread::Thread(u64 entry, u64 stack_size, u64 arg, u8* name, int id, ThreadManager* mgr) : mgr(mgr) {
    state.pc = entry;
    this->id = id;
    this->name = Helpers::readString(name);
    stack = mgr->allocateStack(stack_size);
    this->stack_size = stack_size;
    state.gprs[1] = STACK_REGION_START + stack_size;
}