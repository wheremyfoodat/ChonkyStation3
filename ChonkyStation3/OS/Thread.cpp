#include "Thread.hpp"
#include "ThreadManager.hpp"
#include "PlayStation3.hpp"


Thread::Thread(u64 entry, u64 stack_size, u64 arg, const u8* name, u32 id, u32 tls_vaddr, u32 tls_filesize, u32 tls_memsize, ThreadManager* mgr) : mgr(mgr) {
    const u32 real_entry = mgr->ps3->mem.read<u32>(entry);
    this->id = id;
    this->name = Helpers::readString(name);
    stack = mgr->allocateStack(stack_size + 8);
    this->stack_size = stack_size + 8;  // Add 8 because we simulate 1 empty argument and 1 of whatever env is

    const u64 sp = STACK_REGION_START + stack_size + 8;
    state.pc = real_entry;
    state.gprs[0] = state.pc;
    state.gprs[1] = sp - 12;
    state.gprs[2] = mgr->ps3->mem.read<u32>(entry + 4);

    if (tls_vaddr) {
        // Initialize TLS
        mgr->ps3->module_manager.sysThread.initializeTLS(id, tls_vaddr, tls_filesize, tls_memsize, state);
    }

    // Arguments
    state.gprs[3] = arg;  // argc
    state.gprs[4] = sp - 4;  // argv
    // Fake 1 argument
    u32 empty = 0;
    std::memcpy(mgr->ps3->mem.ram.getPtrPhys(stack - 4), &empty, 4);

    // Environment (environment variables...? That's what I gather from PSL1GHT)
    state.gprs[5] = sp - 8;  // env pointer
    state.gprs[6] = 1;  // env count
    std::memcpy(mgr->ps3->mem.ram.getPtrPhys(stack - 8), &empty, 4);

    state.gprs[7] = id;
    state.gprs[8] = tls_vaddr;
    state.gprs[9] = tls_filesize;
    state.gprs[10] = tls_memsize;
    state.gprs[12] = PAGE_SIZE;

    state.gprs[28] = state.gprs[4];
    state.gprs[29] = state.gprs[3];
    state.gprs[31] = state.gprs[5];
}

// We put the reschedule on the scheduler instead of having it happen instantly because it would break
// if a reschedule is called in the middle of an HLE function
void Thread::reschedule(u64 cycles) {
    mgr->ps3->scheduler.push(std::bind(&ThreadManager::reschedule, mgr), mgr->ps3->curr_block_cycles + cycles, "thread reschedule");
    mgr->ps3->forceSchedulerUpdate();
}

void Thread::sleep(u64 us) {
    const u64 cycles = Scheduler::uSecondsToCycles(us);
    mgr->ps3->scheduler.push(std::bind(&Thread::wakeUp, this), mgr->ps3->curr_block_cycles + cycles, "thread wakeup");
    status = THREAD_STATUS::Sleeping;
    reschedule();
    log("Sleeping thread %d for %d us\n", id, us);
}

void Thread::sleepForCycles(u64 cycles) {
    mgr->ps3->scheduler.push(std::bind(&Thread::wakeUp, this), mgr->ps3->curr_block_cycles + cycles, "thread wakeup");
    status = THREAD_STATUS::Sleeping;
    reschedule();
    log("Sleeping thread %d for %d cycles\n", id, cycles);
}

void Thread::wakeUp() {
    status = THREAD_STATUS::Running;
    reschedule();
    log("Woke up thread %d\n", id);
}

void Thread::exit() {
    status = THREAD_STATUS::Terminated;
    reschedule();
    log("Thread %d exited\n", id);
}