#include "SPUThreadManager.hpp"
#include "PlayStation3.hpp"


SPUThread* SPUThreadManager::createThread(std::string name) {
    threads.push_back({ ps3, name });
    return &threads.back();
}