#include "ThreadManager.hpp"
#include "PlayStation3.hpp"


Thread* ThreadManager::createThread(u64 entry, u64 stack_size, u64 arg, const u8* name, u32 tls_vaddr, u32 tls_filesize, u32 tls_memsize, bool is_start_thread) {
    threads.push_back({ entry, stack_size, arg, name, next_thread_id++, tls_vaddr, tls_filesize, tls_memsize, this});
    // If this is the first thread we create, set 
    // current_thread to point to this thread and initialize ppu
    if (is_start_thread) {
        current_thread_id = threads.back().id;
        mapStack(threads.back());
        ps3->ppu->state = getCurrentThread()->state;
    }
    
    printf("Created thread %d \"%s\"\n", threads.back().id, threads.back().name.c_str());

    return &threads.back();
}

void ThreadManager::contextSwitch(Thread& thread) {
    Thread* current_thread = getCurrentThread();
    if (current_thread->id == thread.id) return;

    printf("Switched from thread %s to thread %s\n", current_thread->name.c_str(), thread.name.c_str());
    current_thread->state = ps3->ppu->state;
    ps3->ppu->state = thread.state;
    
    current_thread_id = thread.id;
    mapStack(thread);
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

    // TODO: check that there is a "next event"
    // If there isn't something bad happened
    if (!found_thread)
        ps3->skipToNextEvent();
}

Thread* ThreadManager::getCurrentThread() {
    return getThreadByID(current_thread_id);
}

Thread* ThreadManager::getThreadByID(u32 id) {
    Thread* thread = nullptr;
    for (auto& i : threads) {
        if (i.id == id) {
            thread = &i;
            break;
        }
    }
    return thread;
}

u64 ThreadManager::allocateStack(u64 stack_size) {
    return ps3->mem.ram.allocPhys(stack_size);
}

void ThreadManager::mapStack(Thread& thread) {
    // Unmap current stack region
    ps3->mem.ram.unmap(STACK_REGION_START);
    ps3->mem.ram.mmap(STACK_REGION_START, thread.stack, thread.stack_size);
}

void ThreadManager::setTLS(u32 tls_vaddr, u32 tls_filesize, u32 tls_memsize) {
    this->tls_vaddr = tls_vaddr;
    this->tls_filesize = tls_filesize;
    this->tls_memsize = tls_memsize;
}

u32 ThreadManager::allocTLS(u32 tls_size) {
    return ps3->mem.ram.alloc(tls_size)->vaddr;
}