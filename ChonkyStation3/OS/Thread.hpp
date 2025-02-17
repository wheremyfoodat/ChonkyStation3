#pragma once

#include <common.hpp>
#include <logger.hpp>

#include <PPUTypes.hpp>
#include <MemoryConstants.hpp>


using namespace PPUTypes;

// Circular dependency
class ThreadManager;

class Thread {
public:
    Thread(u64 entry, u64 stack_size, u64 arg, s32 prio, const u8* name, u32 id, u32 tls_vaddr, u32 tls_filesize, u32 tls_memsize, ThreadManager* mgr);
    ThreadManager* mgr;
    State state;
    s32 prio;
    u64 stack;
    u64 stack_size;
    std::vector<u64> args;
    std::vector<u64> env;
    std::string name;
    u32 id;

    // Used by the thread manager.
    // If this thread is allowed to execute a number of times that exceeds a limit set in the thread manager, this
    // flag will be set to allow other threads to execute.
    bool low_prio = false;
    u64 exec_cnt = 0;

    void addArg(u64 arg);
    void finalizeArgs();
    void addEnv(u64 env);
    void finalizeEnv();
    void finalizeArgsAndEnv();  // To be called after finalizeArgs and finalizeEnv (used to align the stack)

    MAKE_LOG_FUNCTION(log, thread);

    enum class THREAD_STATUS {
        Running,
        Sleeping,
        Waiting,
        Terminated
    };
    THREAD_STATUS status = THREAD_STATUS::Running;

    void reschedule(u64 cycles = 0);
    void sleep(u64 us);
    void sleepForCycles(u64 cycles);
    void wait();
    void wakeUp();
    void exit();

    static std::string threadStatusToString(THREAD_STATUS status) {
        switch (status) {
        case THREAD_STATUS::Running:    return "Running";
        case THREAD_STATUS::Sleeping:   return "Sleeping";
        case THREAD_STATUS::Waiting:    return "Waiting";
        case THREAD_STATUS::Terminated: return "Terminated";
        }
    }
};