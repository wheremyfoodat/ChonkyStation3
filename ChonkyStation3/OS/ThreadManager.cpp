#include "ThreadManager.hpp"
#include "PlayStation3.hpp"


Thread* ThreadManager::createThread(u64 entry, u64 stack_size, u64 arg, u8* name, u32 tls_vaddr, u32 tls_filesize, u32 tls_memsize, bool is_start_thread = false) {
    threads.push_back({ entry, stack_size, arg, name, next_thread_id++, tls_vaddr, tls_filesize, tls_memsize, this});
    // If this is the first thread we create, set current_thread to point to this thread and initialize ppu
    if (is_start_thread) {
        current_thread = &threads.back();
        mapStack(threads.back());
        ps3->ppu->state = current_thread->state;
    }
    
    printf("Created thread \"%s\"\n", threads.back().name.c_str());
    //ps3->ppu->printState();

    return &threads.back();
}

void ThreadManager::contextSwitch(Thread& thread) {
    if (current_thread->id == thread.id) return;

    printf("Switched from thread %s to thread %s\n", current_thread->name.c_str(), thread.name.c_str());
    current_thread->state = ps3->ppu->state;
    ps3->ppu->state = thread.state;
    
    current_thread = &thread;
    mapStack(thread);
    // TODO: tls
}

void ThreadManager::reschedule() {
    bool found_thread = false;
    for (auto& i : threads) {
        if (i.status == Thread::THREAD_STATUS::Running) {
            found_thread = true;
            contextSwitch(i);
            break;
        }
    }

    if (!found_thread)
        ps3->skipToNextEvent();
}

u64 ThreadManager::allocateStack(u64 stack_size) {
    return ps3->mem.ram.allocPhys(stack_size);
}

void ThreadManager::mapStack(Thread& thread) {
    // Unmap current stack region
    ps3->mem.ram.unmap(STACK_REGION_START);
    ps3->mem.ram.mmap(STACK_REGION_START, thread.stack, thread.stack_size);
}

u32 ThreadManager::allocTLS(u32 tls_size) {
    return ps3->mem.ram.alloc(tls_size)->vaddr;
}