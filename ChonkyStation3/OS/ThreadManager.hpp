#pragma once

#include <common.hpp>

#include <Thread.hpp>


// Circular dependency
class PlayStation3;

class ThreadManager {
public:
    ThreadManager(PlayStation3* ps3) : ps3(ps3) {
        // Avoid reallocations
        // My code relies on this... I know it's bad. I pass pointers to Thread objects to the scheduler, if we reallocate
        // the pointers will become invalid
        threads.reserve(128);   // More than enough
    }
    PlayStation3* ps3;

    Thread* createThread(u64 entry, u64 stack_size, u64 arg, const u8* name, u32 tls_vaddr, u32 tls_filesize, u32 tls_memsize, bool is_start_thread = false);
    void contextSwitch(Thread& thread);
    Thread* getCurrentThread();
    Thread* getThreadByID(u32 id);
    void reschedule();
    std::vector<Thread> threads;
    u64 current_thread_id;

    u64 allocateStack(u64 stack_size);
    void mapStack(Thread& thread);

    void setTLS(u32 tls_vaddr, u32 tls_filesize, u32 tls_memsize);
    u32 allocTLS(u32 size);

    u32 tls_vaddr;
    u32 tls_filesize;
    u32 tls_memsize;

private:
    u32 next_thread_id = 0x10000;
    u32 next_tls_addr = TLS_REGION_START;
};