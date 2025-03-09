#pragma once

#include <common.hpp>

#include <SPUThread.hpp>


// Circular dependency
class PlayStation3;

class SPUThreadManager {
public:
    SPUThreadManager(PlayStation3* ps3) : ps3(ps3) {}
    PlayStation3* ps3;

    SPUThread* createThread(std::string name);
    void contextSwitch(SPUThread& thread);
    SPUThread* getCurrentThread();
    SPUThread* getThreadByID(u32 id);
    void reschedule();

    std::vector<SPUThread> threads;
    u64 current_thread_id = 0;

private:
    MAKE_LOG_FUNCTION(log, thread_spu);
};