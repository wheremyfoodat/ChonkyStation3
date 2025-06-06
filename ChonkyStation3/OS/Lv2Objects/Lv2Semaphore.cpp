#include "Lv2Semaphore.hpp"
#include "PlayStation3.hpp"


void Lv2Semaphore::post(u32 val) {
    this->val += val;

    while (wait_list.size()) {
        if (this->val == 0) break;
        // Wake up thread
        Thread* t = ps3->thread_manager.getThreadByID(wait_list.front());
        t->wakeUp();
        wait_list.pop();
        // Decrement semaphore
        this->val--;
    }
}

void Lv2Semaphore::wait() {
    if (val > 0)
        val--;
    else {
        printf("sema %d\n", handle());
        ps3->thread_manager.getCurrentThread()->wait(std::format("sema {}", handle()));
        wait_list.push(ps3->thread_manager.getCurrentThread()->id);
    } 
}
