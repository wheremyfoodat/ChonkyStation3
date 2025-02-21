#include "Lv2Cond.hpp"
#include "PlayStation3.hpp"


bool Lv2Cond::signal(PlayStation3* ps3) {
    Lv2Mutex* mtx = ps3->lv2_obj.get<Lv2Mutex>(mutex_id);

    // Forcefully lock the lwmutex
    if (mtx->owner != ps3->thread_manager.getCurrentThread()->id) {
        //printf("Tried to signal lwcond but the thread did not own the lwmutex (was owned by %s), waiting...\n", ps3->thread_manager.getThreadByID(mtx->owner)->name.c_str());
        // Busy wait
        ps3->thread_manager.getCurrentThread()->sleep(50);
        // We decrease PC so that when the thread is woken up this function will be called again (I know it sucks a bit)
        ps3->ppu->state.pc -= 4;
        return false;
    }

    if (!wait_list.empty()) {
        // Wake up thread
        Thread* t = ps3->thread_manager.getThreadByID(wait_list.front());
        t->wakeUp();
        wait_list.pop();
        // Make the thread reacquire the mutex
        // Temporarily switch to the other thread to lock the mutex
        const auto curr_thread = ps3->thread_manager.getCurrentThread()->id;
        ps3->thread_manager.contextSwitch(*t);
        mtx->lock(ps3);
        ps3->thread_manager.contextSwitch(*ps3->thread_manager.getThreadByID(curr_thread));
    }
    else {
        //signalled = true;
    }

    return true;
}

bool Lv2Cond::wait(PlayStation3* ps3) {
    if (signalled) {
        signalled = false;
    }
    else {
        Lv2Mutex* mtx = ps3->lv2_obj.get<Lv2Mutex>(mutex_id);
        Thread* curr_thread = ps3->thread_manager.getCurrentThread();

        // Check if the current thread is the owner of the mutex
        if (mtx->owner != curr_thread->id)
            return false;

        curr_thread->wait();
        wait_list.push(curr_thread->id);

        // Release the mutex while we are waiting
        mtx->unlock(ps3);

        //printf("Thread %d \"%s\" is waiting on cond variable \"%s\"...\n", curr_thread->id, curr_thread->name.c_str(), name.c_str());
    }

    return true;
}