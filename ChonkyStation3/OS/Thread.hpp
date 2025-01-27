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
    Thread(u64 entry, u64 stack_size, u64 arg, const u8* name, u32 id, u32 tls_vaddr, u32 tls_filesize, u32 tls_memsize, ThreadManager* mgr);
    ThreadManager* mgr;
    State state;
    u64 stack;
    u64 stack_size;
    std::string name;
    u32 id;

    MAKE_LOG_FUNCTION(log, thread);

    enum class THREAD_STATUS {
        Running,
        Sleeping,
        Terminated
    };
    THREAD_STATUS status = THREAD_STATUS::Running;

    void reschedule(u64 cycles = 0);
    void sleep(u64 us);
    void wakeUp();
    void exit();

    static std::string threadStatusToString(THREAD_STATUS status) {
        switch (status) {
        case THREAD_STATUS::Running:    return "Running";
        case THREAD_STATUS::Sleeping:   return "Sleeping";
        case THREAD_STATUS::Terminated: return "Terminated";
        }
    }
};