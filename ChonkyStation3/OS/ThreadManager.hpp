#pragma once

#include <common.hpp>
#include <Thread.hpp>


// Circular dependency
class PlayStation3;

class ThreadManager {
public:
    ThreadManager(PlayStation3* ps3) : ps3(ps3) {}
    PlayStation3* ps3;

    Thread* createThread(u64 entry, u64 stack_size, u64 arg, u8* name, u32 tls_vaddr, u32 tls_filesize, u32 tls_memsize, bool is_start_thread);
    void contextSwitch(Thread& thread);
    void reschedule();
    std::vector<Thread> threads;
    Thread* current_thread;

    u64 allocateStack(u64 stack_size);
    void mapStack(Thread& thread);

    u32 allocTLS(u32 size);
private:
    u32 next_thread_id = 0x10000;
    u32 next_tls_addr = TLS_REGION_START;
};