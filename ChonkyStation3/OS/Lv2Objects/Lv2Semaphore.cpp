#include "Lv2Semaphore.hpp"
#include "PlayStation3.hpp"


void Lv2Semaphore::post(u32 val) {
    this->val = val;
    // TODO: Release threads waiting on this semaphore
}

void Lv2Semaphore::wait() {
    if (val > 0)
        val--;
    else {
        obj->ps3->thread_manager.getCurrentThread()->wait();
    }
}