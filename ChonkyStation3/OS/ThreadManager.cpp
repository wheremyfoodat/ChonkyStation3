#include "ThreadManager.hpp"
#include "PlayStation3.hpp"


Thread* ThreadManager::createThread(u64 entry, u64 stack_size, u64 arg, const u8* name, u32 tls_vaddr, u32 tls_filesize, u32 tls_memsize, bool is_start_thread, std::string executable_path) {
    threads.push_back({ entry, stack_size, arg, name, next_thread_id++, tls_vaddr, tls_filesize, tls_memsize, this});
    // If this is the first thread we create, set 
    // current_thread to point to this thread and initialize ppu
    if (is_start_thread) {
        current_thread_id = threads.back().id;
        mapStack(threads.back());
        ps3->ppu->state = getCurrentThread()->state;

        // argc and argv
        auto entry = ps3->mem.alloc(1_MB);
        ps3->mem.write<u64>(ps3->ppu->state.gprs[1], entry->vaddr);
        std::memcpy(ps3->mem.getPtr(entry->vaddr), executable_path.c_str(), executable_path.length()); // argv[0] should be executable path
        ps3->ppu->state.gprs[3] = 1; // argc
        ps3->ppu->state.gprs[4] = ps3->ppu->state.gprs[1];
        ps3->ppu->state.gprs[1] -= 8;
    }
    
    printf("Created thread %d \"%s\" (entry: 0x%08x)\n", threads.back().id, threads.back().name.c_str(), (u32)threads.back().state.pc);

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
    //printf("Rescheduling...\n");

    do {
        for (auto& i : threads) {
            //printf("Thread %s (%d): %s", i.name.c_str(), i.id, Thread::threadStatusToString(i.status).c_str());
            if (i.status == Thread::THREAD_STATUS::Running) {
                //printf(" (switching to this thread)\n");
                found_thread = true;
                contextSwitch(i);
                break;
            }
            //printf("\n");
        }

        if (!found_thread)
            ps3->skipToNextEvent(); // Will panic if there are no events
    } while (!found_thread);
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
    return ps3->mem.ram.allocPhys(stack_size)->start;
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