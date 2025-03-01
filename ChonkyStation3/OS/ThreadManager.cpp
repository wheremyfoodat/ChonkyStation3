#include "ThreadManager.hpp"
#include "PlayStation3.hpp"


Thread* ThreadManager::createThread(u64 entry, u64 stack_size, u64 arg, s32 prio, const u8* name, u32 tls_vaddr, u32 tls_filesize, u32 tls_memsize, bool is_start_thread, std::string executable_path) {
    threads.push_back({ entry, stack_size, arg, prio, name, next_thread_id++, tls_vaddr, tls_filesize, tls_memsize, this });
    // If this is the first thread we create, set 
    // current_thread to point to this thread and initialize ppu
    if (is_start_thread) {
        current_thread_id = threads.back().id;

        // argc and argv
        auto data = ps3->mem.alloc(1_MB);
        std::memcpy(ps3->mem.getPtr(data->vaddr), executable_path.c_str(), executable_path.length());

        threads.back().addArg(data->vaddr);    // argv[0] should be executable path
        threads.back().finalizeArgs();
        threads.back().addEnv(0);
        threads.back().finalizeEnv();
        threads.back().finalizeArgsAndEnv();

        ps3->ppu->state = getCurrentThread()->state;
    }
    else {
        threads.back().finalizeArgsAndEnv();
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
}

// TODO: There are probably better ways to handle thread scheduling.
//       This is what I managed to come up with on my own.
void ThreadManager::reschedule() {
    bool found_thread = false;
    //printf("Rescheduling...\n");

    int curr_thread = 0;
    while (threads[curr_thread].id != getCurrentThread()->id) curr_thread++;

    do {
        Thread* switch_to = nullptr;
        for (int i = 0; i < threads.size() - 1; i++) {
            Thread& t = threads[(curr_thread + 1 + i) % threads.size()];    // +1 because we want to begin searching from the thread after the current thread
            //printf("Thread %s (%d): %s\n", t.name.c_str(), t.id, Thread::threadStatusToString(t.status).c_str());

            // Does the thread have low priority?
            if (t.low_prio) continue;

            if (t.status == Thread::ThreadStatus::Running) {
                if (!switch_to) {
                    switch_to = &t;
                    found_thread = true;
                }
                else if (t.prio < switch_to->prio) {
                    switch_to = &t;
                    found_thread = true;
                }
            }
        }
        
        // If we found a runnable thread, switch to it
        if (found_thread) {
            contextSwitch(*switch_to);
            switch_to->exec_cnt++;
            if (switch_to->exec_cnt > 4)
                switch_to->low_prio = true;
        }

        // If we did NOT find a runnable thread:
        // - Try to find low priority threads and set them back to high priority.
        // - If we did not find any low priority threads:
        //   - If our current thread was running, keep running it.
        //   - If our current thread was NOT running, skip to the next event and hope a thread wakes up.
        //   - If there is no next event, something is wrong. The emulator will panic.
        else {
            bool found_low_prio = false;
            for (auto& i : threads) {
                if (i.low_prio) {
                    i.low_prio = false;
                    found_low_prio = true;
                }
            }

            if (!found_low_prio) {
                if (getCurrentThread()->status == Thread::ThreadStatus::Running) break;
                else ps3->skipToNextEvent(); // Will panic if there are no events
            }
        }
    } while (!found_thread);
}

void ThreadManager::setAllHighPriority() {
    for (auto& i : threads)
        i.low_prio = false;
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
    return ps3->mem.stack.alloc(stack_size)->vaddr;
}

void ThreadManager::setTLS(u32 tls_vaddr, u32 tls_filesize, u32 tls_memsize) {
    this->tls_vaddr = tls_vaddr;
    this->tls_filesize = tls_filesize;
    this->tls_memsize = tls_memsize;
}

u32 ThreadManager::allocTLS(u32 tls_size) {
    return ps3->mem.ram.alloc(tls_size)->vaddr;
}