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
    std::vector<SPUThread> threads;
    u64 current_thread_id;
};