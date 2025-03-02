#include "Lv2SPUThreadGroup.hpp"
#include "PlayStation3.hpp"


// Returns true if the group was started successfully, false otherwise
bool Lv2SPUThreadGroup::start(PlayStation3* ps3) {
    if (started) return false;

    // TODO: Check if all the threads in this group are ready
    for (auto& i : threads) {
        ps3->spu_thread_manager.getThreadByID(i)->status = SPUThread::ThreadStatus::Running;
    }

    ps3->spu_thread_manager.reschedule();
    started = true;
    return true;
}

std::string Lv2SPUThreadGroup::getName(PlayStation3* ps3) {
    return Helpers::readString(ps3->mem.getPtr(attr->name_ptr));
}