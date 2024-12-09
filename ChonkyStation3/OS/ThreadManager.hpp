#pragma once

#include <common.hpp>
#include <Thread.hpp>


// Circular dependency
class PlayStation3;

class ThreadManager {
public:
    ThreadManager(PlayStation3* ps3) : ps3(ps3) {}
    PlayStation3* ps3;

    void createThread(u64 entry, u64 stack_size, u64 arg, u8* name, bool is_start_thread);
    void contextSwitch(Thread& thread);
    std::vector<Thread> threads;
    Thread* current_thread;

    u64 allocateStack(u64 stack_size);
    void mapStack(Thread& thread);
private:
    int next_thread_id = 0;
};